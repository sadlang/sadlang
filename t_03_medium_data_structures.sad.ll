; ModuleID = '03_medium_data_structures'
source_filename = "03_medium_data_structures"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [25 x i8] c"\D8\A7\D9\84\D9\85\D9\83\D8\AF\D8\B3 \D9\85\D9\85\D8\AA\D9\84\D8\A6!\00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.1 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.3 = private unnamed_addr constant [23 x i8] c"\D8\A7\D9\84\D9\85\D9\83\D8\AF\D8\B3 \D9\81\D8\A7\D8\B1\D8\BA!\00", align 1
@str.const.7 = private unnamed_addr constant [13 x i8] c"  \D8\B9\D8\AF\D8\A7\D8\AF [\00", align 1
@str.const.8 = private unnamed_addr constant [4 x i8] c"]: \00", align 1
@str.const.9 = private unnamed_addr constant [2 x i8] c"/\00", align 1
@str.const.10 = private unnamed_addr constant [17 x i8] c" (\D8\B2\D9\8A\D8\A7\D8\AF\D8\A7\D8\AA: \00", align 1
@str.const.11 = private unnamed_addr constant [2 x i8] c")\00", align 1
@str.const.14 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.15 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.16 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.17 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.18 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.19 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.20 = private unnamed_addr constant [23 x i8] c"\D8\A7\D9\84\D8\B3\D8\AC\D9\84 \D9\85\D9\85\D8\AA\D9\84\D8\A6!\00", align 1
@str.const.24 = private unnamed_addr constant [4 x i8] c"  {\00", align 1
@str.const.28 = private unnamed_addr constant [5 x i8] c"    \00", align 1
@str.const.29 = private unnamed_addr constant [3 x i8] c": \00", align 1
@str.const.43 = private unnamed_addr constant [4 x i8] c"  }\00", align 1
@str.const.47 = private unnamed_addr constant [60 x i8] c"=== \D9\85\D8\A7\D9\83\D8\B1\D9\88 \D8\A8\D9\86\D8\A7\D8\A1 \D8\A7\D9\84\D9\87\D9\8A\D8\A7\D9\83\D9\84 \D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D9\8A\D8\A9 ===\00", align 1
@str.const.51 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.55 = private unnamed_addr constant [21 x i8] c"--- \D8\A7\D9\84\D9\85\D9\83\D8\AF\D8\B3 ---\00", align 1
@str.const.59 = private unnamed_addr constant [13 x i8] c"\D8\A7\D9\84\D8\B9\D9\85\D9\82: \00", align 1
@str.const.62 = private unnamed_addr constant [13 x i8] c"\D8\A7\D9\84\D9\82\D9\85\D8\A9: \00", align 1
@str.const.65 = private unnamed_addr constant [13 x i8] c"\D9\85\D8\B3\D8\AD\D9\88\D8\A8: \00", align 1
@str.const.68 = private unnamed_addr constant [31 x i8] c"\D8\A7\D9\84\D8\B9\D9\85\D9\82 \D8\A8\D8\B9\D8\AF \D8\A7\D9\84\D8\B3\D8\AD\D8\A8: \00", align 1
@str.const.71 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.75 = private unnamed_addr constant [25 x i8] c"--- \D8\A7\D9\84\D8\B9\D8\AF\D8\A7\D8\AF\D8\A7\D8\AA ---\00", align 1
@str.const.79 = private unnamed_addr constant [9 x i8] c"\D8\B2\D9\88\D8\A7\D8\B1\00", align 1
@str.const.80 = private unnamed_addr constant [11 x i8] c"\D8\B7\D9\84\D8\A8\D8\A7\D8\AA\00", align 1
@str.const.81 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.85 = private unnamed_addr constant [23 x i8] c"--- \D8\A7\D9\84\D8\B3\D8\AC\D9\84\D8\A7\D8\AA ---\00", align 1
@str.const.89 = private unnamed_addr constant [11 x i8] c"\D8\A7\D9\84\D8\A7\D8\B3\D9\85\00", align 1
@str.const.90 = private unnamed_addr constant [9 x i8] c"\D8\A3\D8\AD\D9\85\D8\AF\00", align 1
@str.const.91 = private unnamed_addr constant [11 x i8] c"\D8\A7\D9\84\D8\B9\D9\85\D8\B1\00", align 1
@str.const.92 = private unnamed_addr constant [3 x i8] c"25\00", align 1
@str.const.93 = private unnamed_addr constant [15 x i8] c"\D8\A7\D9\84\D9\85\D8\AF\D9\8A\D9\86\D8\A9\00", align 1
@str.const.94 = private unnamed_addr constant [13 x i8] c"\D8\A7\D9\84\D8\B1\D9\8A\D8\A7\D8\B6\00", align 1
@str.const.95 = private unnamed_addr constant [10 x i8] c"\D8\B3\D8\AC\D9\84 1:\00", align 1
@str.const.99 = private unnamed_addr constant [13 x i8] c"\D8\A7\D9\84\D9\85\D9\86\D8\AA\D8\AC\00", align 1
@str.const.100 = private unnamed_addr constant [9 x i8] c"\D9\87\D8\A7\D8\AA\D9\81\00", align 1
@str.const.101 = private unnamed_addr constant [11 x i8] c"\D8\A7\D9\84\D8\B3\D8\B9\D8\B1\00", align 1
@str.const.102 = private unnamed_addr constant [5 x i8] c"2500\00", align 1
@str.const.103 = private unnamed_addr constant [13 x i8] c"\D8\A7\D9\84\D9\83\D9\85\D9\8A\D8\A9\00", align 1
@str.const.104 = private unnamed_addr constant [4 x i8] c"100\00", align 1
@str.const.105 = private unnamed_addr constant [10 x i8] c"\D8\B3\D8\AC\D9\84 2:\00", align 1
@str.const.109 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.113 = private unnamed_addr constant [60 x i8] c"=== \D8\A7\D9\86\D8\AA\D9\87\D9\89 \D8\A8\D9\86\D8\A7\D8\A1 \D8\A7\D9\84\D9\87\D9\8A\D8\A7\D9\83\D9\84 \D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D9\8A\D8\A9 ===\00", align 1

define void @"\D9\85\D9\83\D8\AF\D8\B3.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca i64, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  store i64 0, ptr %alloca2, align 8
  store i64 0, ptr %alloca3, align 8
  store i64 0, ptr %alloca, align 8
  store i64 0, ptr %alloca4, align 8
  store i64 0, ptr %alloca5, align 8
  store i64 0, ptr %alloca6, align 8
  ret void
}

define void @"\D9\85\D9\83\D8\AF\D8\B3.\D8\A7\D8\AF\D9\81\D8\B9"(i64 %self, i64 %"\D9\82\D9\8A\D9\85\D8\A9") {
entry:
  %"\D9\82\D9\8A\D9\85\D8\A92" = alloca i64, align 8
  store i64 %"\D9\82\D9\8A\D9\85\D8\A9", ptr %"\D9\82\D9\8A\D9\85\D8\A92", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  %alloca7 = alloca i64, align 8
  %"%\D9\82\D9\85\D8\A9.load" = load i64, ptr %alloca3, align 8
  %cmpgetmp = icmp sge i64 %"%\D9\82\D9\85\D8\A9.load", 5
  br i1 %cmpgetmp, label %then_0, label %else_2

then_0:                                           ; preds = %entry
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_1

else_2:                                           ; preds = %entry
  %loadtmp = load i64, ptr %alloca3, align 8
  br label %match.case0.test_4

merge_1:                                          ; preds = %match.end_3, %then_0
  ret void

match.end_3:                                      ; preds = %match.case4.body_13, %match.case4.test_12, %match.case3.body_11, %match.case2.body_9, %match.case1.body_7, %match.case0.body_5
  %"%\D9\82\D9\85\D8\A9.load8" = load i64, ptr %alloca3, align 8
  %addtmp = add i64 %"%\D9\82\D9\85\D8\A9.load8", 1
  store i64 %addtmp, ptr %alloca3, align 8
  br label %merge_1

match.case0.test_4:                               ; preds = %else_2
  %cmpeqtmp = icmp eq i64 %loadtmp, 0
  br i1 %cmpeqtmp, label %match.case0.body_5, label %match.case1.test_6

match.case0.body_5:                               ; preds = %match.case0.test_4
  %"%\D9\82\D9\8A\D9\85\D8\A9.load" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A92", align 8
  store i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load", ptr %alloca4, align 8
  br label %match.end_3

match.case1.test_6:                               ; preds = %match.case0.test_4
  %cmpeqtmp9 = icmp eq i64 %loadtmp, 1
  br i1 %cmpeqtmp9, label %match.case1.body_7, label %match.case2.test_8

match.case1.body_7:                               ; preds = %match.case1.test_6
  %"%\D9\82\D9\8A\D9\85\D8\A9.load10" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A92", align 8
  store i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load10", ptr %alloca, align 8
  br label %match.end_3

match.case2.test_8:                               ; preds = %match.case1.test_6
  %cmpeqtmp11 = icmp eq i64 %loadtmp, 2
  br i1 %cmpeqtmp11, label %match.case2.body_9, label %match.case3.test_10

match.case2.body_9:                               ; preds = %match.case2.test_8
  %"%\D9\82\D9\8A\D9\85\D8\A9.load12" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A92", align 8
  store i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load12", ptr %alloca5, align 8
  br label %match.end_3

match.case3.test_10:                              ; preds = %match.case2.test_8
  %cmpeqtmp13 = icmp eq i64 %loadtmp, 3
  br i1 %cmpeqtmp13, label %match.case3.body_11, label %match.case4.test_12

match.case3.body_11:                              ; preds = %match.case3.test_10
  %"%\D9\82\D9\8A\D9\85\D8\A9.load14" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A92", align 8
  store i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load14", ptr %alloca6, align 8
  br label %match.end_3

match.case4.test_12:                              ; preds = %match.case3.test_10
  %cmpeqtmp15 = icmp eq i64 %loadtmp, 4
  br i1 %cmpeqtmp15, label %match.case4.body_13, label %match.end_3

match.case4.body_13:                              ; preds = %match.case4.test_12
  %"%\D9\82\D9\8A\D9\85\D8\A9.load16" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A92", align 8
  store i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load16", ptr %alloca7, align 8
  br label %match.end_3
}

define i64 @"\D9\85\D9\83\D8\AF\D8\B3.\D8\A7\D8\B3\D8\AD\D8\A8"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca i64, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  %"%\D9\82\D9\85\D8\A9.load" = load i64, ptr %alloca2, align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\82\D9\85\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_14, label %merge_15

then_14:                                          ; preds = %entry
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0

merge_15:                                         ; preds = %entry
  %"%\D9\82\D9\85\D8\A9.load7" = load i64, ptr %alloca2, align 8
  %subtmp = sub i64 %"%\D9\82\D9\85\D8\A9.load7", 1
  store i64 %subtmp, ptr %alloca2, align 8
  %loadtmp = load i64, ptr %alloca2, align 8
  br label %match.case0.test_17

match.end_16:                                     ; preds = %match.case5.test_27
  ret i64 0

match.case0.test_17:                              ; preds = %merge_15
  %cmpeqtmp8 = icmp eq i64 %loadtmp, 0
  br i1 %cmpeqtmp8, label %match.case0.body_18, label %match.case1.test_19

match.case0.body_18:                              ; preds = %match.case0.test_17
  %loadtmp9 = load i64, ptr %alloca3, align 8
  ret i64 %loadtmp9

match.case1.test_19:                              ; preds = %match.case0.test_17
  %cmpeqtmp10 = icmp eq i64 %loadtmp, 1
  br i1 %cmpeqtmp10, label %match.case1.body_20, label %match.case2.test_21

match.case1.body_20:                              ; preds = %match.case1.test_19
  %loadtmp11 = load i64, ptr %alloca, align 8
  ret i64 %loadtmp11

match.case2.test_21:                              ; preds = %match.case1.test_19
  %cmpeqtmp12 = icmp eq i64 %loadtmp, 2
  br i1 %cmpeqtmp12, label %match.case2.body_22, label %match.case3.test_23

match.case2.body_22:                              ; preds = %match.case2.test_21
  %loadtmp13 = load i64, ptr %alloca4, align 8
  ret i64 %loadtmp13

match.case3.test_23:                              ; preds = %match.case2.test_21
  %cmpeqtmp14 = icmp eq i64 %loadtmp, 3
  br i1 %cmpeqtmp14, label %match.case3.body_24, label %match.case4.test_25

match.case3.body_24:                              ; preds = %match.case3.test_23
  %loadtmp15 = load i64, ptr %alloca5, align 8
  ret i64 %loadtmp15

match.case4.test_25:                              ; preds = %match.case3.test_23
  %cmpeqtmp16 = icmp eq i64 %loadtmp, 4
  br i1 %cmpeqtmp16, label %match.case4.body_26, label %match.case5.test_27

match.case4.body_26:                              ; preds = %match.case4.test_25
  %loadtmp17 = load i64, ptr %alloca6, align 8
  ret i64 %loadtmp17

match.case5.test_27:                              ; preds = %match.case4.test_25
  br i1 true, label %match.case5.body_28, label %match.end_16

match.case5.body_28:                              ; preds = %match.case5.test_27
  ret i64 0
}

define i64 @"\D9\85\D9\83\D8\AF\D8\B3.\D8\A7\D9\86\D8\B8\D8\B1_\D9\82\D9\85\D8\A9"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca i64, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  %"%\D9\82\D9\85\D8\A9.load" = load i64, ptr %alloca2, align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\82\D9\85\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_29, label %merge_30

then_29:                                          ; preds = %entry
  ret i64 0

merge_30:                                         ; preds = %entry
  %"%\D9\82\D9\85\D8\A9.load7" = load i64, ptr %alloca2, align 8
  %subtmp = sub i64 %"%\D9\82\D9\85\D8\A9.load7", 1
  %alloca8 = alloca i64, align 8
  store i64 %subtmp, ptr %alloca8, align 8
  %loadtmp = load i64, ptr %alloca8, align 8
  br label %match.case0.test_32

match.end_31:                                     ; preds = %match.case5.test_42
  ret i64 0

match.case0.test_32:                              ; preds = %merge_30
  %cmpeqtmp9 = icmp eq i64 %loadtmp, 0
  br i1 %cmpeqtmp9, label %match.case0.body_33, label %match.case1.test_34

match.case0.body_33:                              ; preds = %match.case0.test_32
  %loadtmp10 = load i64, ptr %alloca3, align 8
  ret i64 %loadtmp10

match.case1.test_34:                              ; preds = %match.case0.test_32
  %cmpeqtmp11 = icmp eq i64 %loadtmp, 1
  br i1 %cmpeqtmp11, label %match.case1.body_35, label %match.case2.test_36

match.case1.body_35:                              ; preds = %match.case1.test_34
  %loadtmp12 = load i64, ptr %alloca, align 8
  ret i64 %loadtmp12

match.case2.test_36:                              ; preds = %match.case1.test_34
  %cmpeqtmp13 = icmp eq i64 %loadtmp, 2
  br i1 %cmpeqtmp13, label %match.case2.body_37, label %match.case3.test_38

match.case2.body_37:                              ; preds = %match.case2.test_36
  %loadtmp14 = load i64, ptr %alloca4, align 8
  ret i64 %loadtmp14

match.case3.test_38:                              ; preds = %match.case2.test_36
  %cmpeqtmp15 = icmp eq i64 %loadtmp, 3
  br i1 %cmpeqtmp15, label %match.case3.body_39, label %match.case4.test_40

match.case3.body_39:                              ; preds = %match.case3.test_38
  %loadtmp16 = load i64, ptr %alloca5, align 8
  ret i64 %loadtmp16

match.case4.test_40:                              ; preds = %match.case3.test_38
  %cmpeqtmp17 = icmp eq i64 %loadtmp, 4
  br i1 %cmpeqtmp17, label %match.case4.body_41, label %match.case5.test_42

match.case4.body_41:                              ; preds = %match.case4.test_40
  %loadtmp18 = load i64, ptr %alloca6, align 8
  ret i64 %loadtmp18

match.case5.test_42:                              ; preds = %match.case4.test_40
  br i1 true, label %match.case5.body_43, label %match.end_31

match.case5.body_43:                              ; preds = %match.case5.test_42
  ret i64 0
}

define i64 @"\D9\85\D9\83\D8\AF\D8\B3.\D8\A7\D9\84\D8\B9\D9\85\D9\82"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca i64, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  %loadtmp = load i64, ptr %alloca2, align 8
  ret i64 %loadtmp
}

define i1 @"\D9\85\D9\83\D8\AF\D8\B3.\D9\81\D8\A7\D8\B1\D8\BA"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca i64, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  %"%\D9\82\D9\85\D8\A9.load" = load i64, ptr %alloca2, align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\82\D9\85\D8\A9.load", 0
  ret i1 %cmpeqtmp
}

define void @"\D8\B9\D8\AF\D8\A7\D8\AF.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85", i64 %"\D8\A7\D9\84\D8\AD\D8\AF") {
entry:
  %"\D8\A7\D9\84\D8\AD\D8\AF3" = alloca i64, align 8
  store i64 %"\D8\A7\D9\84\D8\AD\D8\AF", ptr %"\D8\A7\D9\84\D8\AD\D8\AF3", align 8
  %"\D8\A7\D9\84\D8\A7\D8\B3\D9\852" = alloca ptr, align 8
  store ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85", ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\852", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca4 = alloca ptr, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  store ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\852", ptr %alloca4, align 8
  store i64 0, ptr %alloca, align 8
  %"%\D8\A7\D9\84\D8\AD\D8\AF.load" = load i64, ptr %"\D8\A7\D9\84\D8\AD\D8\AF3", align 8
  store i64 %"%\D8\A7\D9\84\D8\AD\D8\AF.load", ptr %alloca6, align 8
  store i64 0, ptr %alloca5, align 8
  ret void
}

define void @"\D8\B9\D8\AF\D8\A7\D8\AF.\D8\B2\D8\AF"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load" = load i64, ptr %alloca, align 8
  %"%\D8\A3\D9\82\D8\B5\D9\89_\D8\AD\D8\AF.load" = load i64, ptr %alloca4, align 8
  %cmplttmp = icmp slt i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load", %"%\D8\A3\D9\82\D8\B5\D9\89_\D8\AD\D8\AF.load"
  br i1 %cmplttmp, label %then_44, label %merge_45

then_44:                                          ; preds = %entry
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load5" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load5", 1
  store i64 %addtmp, ptr %alloca, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\B2\D9\8A\D8\A7\D8\AF\D8\A7\D8\AA.load" = load i64, ptr %alloca3, align 8
  %addtmp6 = add i64 %"%\D8\B9\D8\AF\D8\AF_\D8\B2\D9\8A\D8\A7\D8\AF\D8\A7\D8\AA.load", 1
  store i64 %addtmp6, ptr %alloca3, align 8
  br label %merge_45

merge_45:                                         ; preds = %then_44, %entry
  ret void
}

define void @"\D8\B9\D8\AF\D8\A7\D8\AF.\D8\A3\D9\86\D9\82\D8\B5"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load" = load i64, ptr %alloca, align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load", 0
  br i1 %cmpgttmp, label %then_46, label %merge_47

then_46:                                          ; preds = %entry
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load5" = load i64, ptr %alloca, align 8
  %subtmp = sub i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load5", 1
  store i64 %subtmp, ptr %alloca, align 8
  br label %merge_47

merge_47:                                         ; preds = %then_46, %entry
  ret void
}

define void @"\D8\B9\D8\AF\D8\A7\D8\AF.\D8\B5\D9\81\D9\91\D8\B1"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  ret void
}

define i64 @"\D8\B9\D8\AF\D8\A7\D8\AF.\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %loadtmp = load i64, ptr %alloca, align 8
  ret i64 %loadtmp
}

define void @"\D8\B9\D8\AF\D8\A7\D8\AF.\D8\A7\D8\B9\D8\B1\D8\B6"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %len1 = call i64 @strlen(ptr @str.const.7)
  %len2 = call i64 @strlen(ptr %alloca2)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.7, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %alloca2, i64 %copylen2)
  %len15 = call i64 @strlen(ptr %concat.buf)
  %len26 = call i64 @strlen(ptr @str.const.8)
  %totallen7 = add i64 %len15, %len26
  %bufsize8 = add i64 %totallen7, 1
  %concat.buf9 = alloca i8, i64 %bufsize8, align 1
  %2 = call ptr @memcpy(ptr %concat.buf9, ptr %concat.buf, i64 %len15)
  %dest210 = getelementptr i8, ptr %concat.buf9, i64 %len15
  %copylen211 = add i64 %len26, 1
  %3 = call ptr @memcpy(ptr %dest210, ptr @str.const.8, i64 %copylen211)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret void
}

define void @"\D8\B3\D8\AC\D9\84_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca2 = alloca i64, align 8
  %alloca3 = alloca ptr, align 8
  %alloca4 = alloca ptr, align 8
  %alloca5 = alloca ptr, align 8
  %alloca6 = alloca ptr, align 8
  %alloca7 = alloca ptr, align 8
  store ptr @str.const.14, ptr %alloca, align 8
  store ptr @str.const.15, ptr %alloca3, align 8
  store ptr @str.const.16, ptr %alloca5, align 8
  store ptr @str.const.17, ptr %alloca7, align 8
  store ptr @str.const.18, ptr %alloca6, align 8
  store ptr @str.const.19, ptr %alloca4, align 8
  store i64 0, ptr %alloca2, align 8
  ret void
}

define void @"\D8\B3\D8\AC\D9\84_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.\D8\B9\D9\8A\D9\91\D9\86_\D8\AD\D9\82\D9\84"(i64 %self, ptr %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD", ptr %"\D9\82\D9\8A\D9\85\D8\A9") {
entry:
  %"\D9\82\D9\8A\D9\85\D8\A93" = alloca ptr, align 8
  store ptr %"\D9\82\D9\8A\D9\85\D8\A9", ptr %"\D9\82\D9\8A\D9\85\D8\A93", align 8
  %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD2" = alloca ptr, align 8
  store ptr %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD", ptr %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD2", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca ptr, align 8
  %alloca6 = alloca ptr, align 8
  %alloca7 = alloca ptr, align 8
  %alloca8 = alloca ptr, align 8
  %alloca9 = alloca ptr, align 8
  %loadtmp = load i64, ptr %alloca4, align 8
  br label %match.case0.test_49

match.end_48:                                     ; preds = %match.case3.body_56, %match.case3.test_55, %match.case2.body_54, %match.case1.body_52, %match.case0.body_50
  %"%\D8\B9\D8\AF\D8\AF_\D8\AD\D9\82\D9\88\D9\84.load" = load i64, ptr %alloca4, align 8
  %cmplttmp = icmp slt i64 %"%\D8\B9\D8\AF\D8\AF_\D8\AD\D9\82\D9\88\D9\84.load", 3
  br i1 %cmplttmp, label %then_57, label %merge_58

match.case0.test_49:                              ; preds = %entry
  %cmpeqtmp = icmp eq i64 %loadtmp, 0
  br i1 %cmpeqtmp, label %match.case0.body_50, label %match.case1.test_51

match.case0.body_50:                              ; preds = %match.case0.test_49
  store ptr %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD2", ptr %alloca, align 8
  store ptr %"\D9\82\D9\8A\D9\85\D8\A93", ptr %alloca5, align 8
  br label %match.end_48

match.case1.test_51:                              ; preds = %match.case0.test_49
  %cmpeqtmp10 = icmp eq i64 %loadtmp, 1
  br i1 %cmpeqtmp10, label %match.case1.body_52, label %match.case2.test_53

match.case1.body_52:                              ; preds = %match.case1.test_51
  store ptr %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD2", ptr %alloca7, align 8
  store ptr %"\D9\82\D9\8A\D9\85\D8\A93", ptr %alloca9, align 8
  br label %match.end_48

match.case2.test_53:                              ; preds = %match.case1.test_51
  %cmpeqtmp11 = icmp eq i64 %loadtmp, 2
  br i1 %cmpeqtmp11, label %match.case2.body_54, label %match.case3.test_55

match.case2.body_54:                              ; preds = %match.case2.test_53
  store ptr %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD2", ptr %alloca8, align 8
  store ptr %"\D9\82\D9\8A\D9\85\D8\A93", ptr %alloca6, align 8
  br label %match.end_48

match.case3.test_55:                              ; preds = %match.case2.test_53
  br i1 true, label %match.case3.body_56, label %match.end_48

match.case3.body_56:                              ; preds = %match.case3.test_55
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.20)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %match.end_48

then_57:                                          ; preds = %match.end_48
  %"%\D8\B9\D8\AF\D8\AF_\D8\AD\D9\82\D9\88\D9\84.load12" = load i64, ptr %alloca4, align 8
  %addtmp = add i64 %"%\D8\B9\D8\AF\D8\AF_\D8\AD\D9\82\D9\88\D9\84.load12", 1
  store i64 %addtmp, ptr %alloca4, align 8
  br label %merge_58

merge_58:                                         ; preds = %then_57, %match.end_48
  ret void
}

define void @"\D8\B3\D8\AC\D9\84_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.\D8\A7\D8\B9\D8\B1\D8\B6"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca2 = alloca i64, align 8
  %alloca3 = alloca ptr, align 8
  %alloca4 = alloca ptr, align 8
  %alloca5 = alloca ptr, align 8
  %alloca6 = alloca ptr, align 8
  %alloca7 = alloca ptr, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.24)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\B9\D8\AF\D8\AF_\D8\AD\D9\82\D9\88\D9\84.load" = load i64, ptr %alloca2, align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B9\D8\AF\D8\AF_\D8\AD\D9\82\D9\88\D9\84.load", 1
  br i1 %cmpgetmp, label %then_59, label %merge_60

then_59:                                          ; preds = %entry
  %len1 = call i64 @strlen(ptr @str.const.28)
  %len2 = call i64 @strlen(ptr %alloca)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %2 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.28, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %3 = call ptr @memcpy(ptr %dest2, ptr %alloca, i64 %copylen2)
  %len18 = call i64 @strlen(ptr %concat.buf)
  %len29 = call i64 @strlen(ptr @str.const.29)
  %totallen10 = add i64 %len18, %len29
  %bufsize11 = add i64 %totallen10, 1
  %concat.buf12 = alloca i8, i64 %bufsize11, align 1
  %4 = call ptr @memcpy(ptr %concat.buf12, ptr %concat.buf, i64 %len18)
  %dest213 = getelementptr i8, ptr %concat.buf12, i64 %len18
  %copylen214 = add i64 %len29, 1
  %5 = call ptr @memcpy(ptr %dest213, ptr @str.const.29, i64 %copylen214)
  %len115 = call i64 @strlen(ptr %concat.buf12)
  %len216 = call i64 @strlen(ptr %alloca3)
  %totallen17 = add i64 %len115, %len216
  %bufsize18 = add i64 %totallen17, 1
  %concat.buf19 = alloca i8, i64 %bufsize18, align 1
  %6 = call ptr @memcpy(ptr %concat.buf19, ptr %concat.buf12, i64 %len115)
  %dest220 = getelementptr i8, ptr %concat.buf19, i64 %len115
  %copylen221 = add i64 %len216, 1
  %7 = call ptr @memcpy(ptr %dest220, ptr %alloca3, i64 %copylen221)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf19)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_60

merge_60:                                         ; preds = %then_59, %entry
  %"%\D8\B9\D8\AF\D8\AF_\D8\AD\D9\82\D9\88\D9\84.load22" = load i64, ptr %alloca2, align 8
  %cmpgetmp23 = icmp sge i64 %"%\D8\B9\D8\AF\D8\AF_\D8\AD\D9\82\D9\88\D9\84.load22", 2
  br i1 %cmpgetmp23, label %then_61, label %merge_62

then_61:                                          ; preds = %merge_60
  %len124 = call i64 @strlen(ptr @str.const.28)
  %len225 = call i64 @strlen(ptr %alloca5)
  %totallen26 = add i64 %len124, %len225
  %bufsize27 = add i64 %totallen26, 1
  %concat.buf28 = alloca i8, i64 %bufsize27, align 1
  %10 = call ptr @memcpy(ptr %concat.buf28, ptr @str.const.28, i64 %len124)
  %dest229 = getelementptr i8, ptr %concat.buf28, i64 %len124
  %copylen230 = add i64 %len225, 1
  %11 = call ptr @memcpy(ptr %dest229, ptr %alloca5, i64 %copylen230)
  %len131 = call i64 @strlen(ptr %concat.buf28)
  %len232 = call i64 @strlen(ptr @str.const.29)
  %totallen33 = add i64 %len131, %len232
  %bufsize34 = add i64 %totallen33, 1
  %concat.buf35 = alloca i8, i64 %bufsize34, align 1
  %12 = call ptr @memcpy(ptr %concat.buf35, ptr %concat.buf28, i64 %len131)
  %dest236 = getelementptr i8, ptr %concat.buf35, i64 %len131
  %copylen237 = add i64 %len232, 1
  %13 = call ptr @memcpy(ptr %dest236, ptr @str.const.29, i64 %copylen237)
  %len138 = call i64 @strlen(ptr %concat.buf35)
  %len239 = call i64 @strlen(ptr %alloca7)
  %totallen40 = add i64 %len138, %len239
  %bufsize41 = add i64 %totallen40, 1
  %concat.buf42 = alloca i8, i64 %bufsize41, align 1
  %14 = call ptr @memcpy(ptr %concat.buf42, ptr %concat.buf35, i64 %len138)
  %dest243 = getelementptr i8, ptr %concat.buf42, i64 %len138
  %copylen244 = add i64 %len239, 1
  %15 = call ptr @memcpy(ptr %dest243, ptr %alloca7, i64 %copylen244)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf42)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_62

merge_62:                                         ; preds = %then_61, %merge_60
  %"%\D8\B9\D8\AF\D8\AF_\D8\AD\D9\82\D9\88\D9\84.load45" = load i64, ptr %alloca2, align 8
  %cmpgetmp46 = icmp sge i64 %"%\D8\B9\D8\AF\D8\AF_\D8\AD\D9\82\D9\88\D9\84.load45", 3
  br i1 %cmpgetmp46, label %then_63, label %merge_64

then_63:                                          ; preds = %merge_62
  %len147 = call i64 @strlen(ptr @str.const.28)
  %len248 = call i64 @strlen(ptr %alloca6)
  %totallen49 = add i64 %len147, %len248
  %bufsize50 = add i64 %totallen49, 1
  %concat.buf51 = alloca i8, i64 %bufsize50, align 1
  %18 = call ptr @memcpy(ptr %concat.buf51, ptr @str.const.28, i64 %len147)
  %dest252 = getelementptr i8, ptr %concat.buf51, i64 %len147
  %copylen253 = add i64 %len248, 1
  %19 = call ptr @memcpy(ptr %dest252, ptr %alloca6, i64 %copylen253)
  %len154 = call i64 @strlen(ptr %concat.buf51)
  %len255 = call i64 @strlen(ptr @str.const.29)
  %totallen56 = add i64 %len154, %len255
  %bufsize57 = add i64 %totallen56, 1
  %concat.buf58 = alloca i8, i64 %bufsize57, align 1
  %20 = call ptr @memcpy(ptr %concat.buf58, ptr %concat.buf51, i64 %len154)
  %dest259 = getelementptr i8, ptr %concat.buf58, i64 %len154
  %copylen260 = add i64 %len255, 1
  %21 = call ptr @memcpy(ptr %dest259, ptr @str.const.29, i64 %copylen260)
  %len161 = call i64 @strlen(ptr %concat.buf58)
  %len262 = call i64 @strlen(ptr %alloca4)
  %totallen63 = add i64 %len161, %len262
  %bufsize64 = add i64 %totallen63, 1
  %concat.buf65 = alloca i8, i64 %bufsize64, align 1
  %22 = call ptr @memcpy(ptr %concat.buf65, ptr %concat.buf58, i64 %len161)
  %dest266 = getelementptr i8, ptr %concat.buf65, i64 %len161
  %copylen267 = add i64 %len262, 1
  %23 = call ptr @memcpy(ptr %dest266, ptr %alloca4, i64 %copylen267)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf65)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_64

merge_64:                                         ; preds = %then_63, %merge_62
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.43)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret void
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.47)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.51)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.55)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca = alloca i64, align 8
  %alloca1 = alloca i64, align 8
  %"%148.load" = load i64, ptr %alloca, align 8
  store i64 %"%148.load", ptr %alloca1, align 8
  %"%\D9\85.load" = load i64, ptr %alloca1, align 8
  call void @"\D9\85\D9\83\D8\AF\D8\B3.\D8\A7\D8\AF\D9\81\D8\B9"(i64 %"%\D9\85.load", i64 10)
  %"%\D9\85.load2" = load i64, ptr %alloca1, align 8
  call void @"\D9\85\D9\83\D8\AF\D8\B3.\D8\A7\D8\AF\D9\81\D8\B9"(i64 %"%\D9\85.load2", i64 20)
  %"%\D9\85.load3" = load i64, ptr %alloca1, align 8
  call void @"\D9\85\D9\83\D8\AF\D8\B3.\D8\A7\D8\AF\D9\81\D8\B9"(i64 %"%\D9\85.load3", i64 30)
  %"%\D9\85.load4" = load i64, ptr %alloca1, align 8
  %calltmp = call i64 @"\D9\85\D9\83\D8\AF\D8\B3.\D8\A7\D9\84\D8\B9\D9\85\D9\82"(i64 %"%\D9\85.load4")
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\85.load5" = load i64, ptr %alloca1, align 8
  %calltmp6 = call i64 @"\D9\85\D9\83\D8\AF\D8\B3.\D8\A7\D9\86\D8\B8\D8\B1_\D9\82\D9\85\D8\A9"(i64 %"%\D9\85.load5")
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\85.load7" = load i64, ptr %alloca1, align 8
  %calltmp8 = call i64 @"\D9\85\D9\83\D8\AF\D8\B3.\D8\A7\D8\B3\D8\AD\D8\A8"(i64 %"%\D9\85.load7")
  %alloca9 = alloca i64, align 8
  store i64 %calltmp8, ptr %alloca9, align 8
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\85.load10" = load i64, ptr %alloca1, align 8
  %calltmp11 = call i64 @"\D9\85\D9\83\D8\AF\D8\B3.\D8\A7\D9\84\D8\B9\D9\85\D9\82"(i64 %"%\D9\85.load10")
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.71)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.75)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca12 = alloca i64, align 8
  %"%173.load" = load i64, ptr %alloca12, align 8
  call void @"\D8\B9\D8\AF\D8\A7\D8\AF.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%173.load", ptr @str.const.79, i64 100)
  %alloca13 = alloca i64, align 8
  %"%173.load14" = load i64, ptr %alloca12, align 8
  store i64 %"%173.load14", ptr %alloca13, align 8
  %alloca15 = alloca i64, align 8
  %"%177.load" = load i64, ptr %alloca15, align 8
  call void @"\D8\B9\D8\AF\D8\A7\D8\AF.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%177.load", ptr @str.const.80, i64 50)
  %alloca16 = alloca i64, align 8
  %"%177.load17" = load i64, ptr %alloca15, align 8
  store i64 %"%177.load17", ptr %alloca16, align 8
  %alloca18 = alloca i64, align 8
  store i64 0, ptr %alloca18, align 8
  br label %while_cond_65

while_cond_65:                                    ; preds = %while_body_66, %entry
  %"%\D8\AA.load" = load i64, ptr %alloca18, align 8
  %cmplttmp = icmp slt i64 %"%\D8\AA.load", 10
  br i1 %cmplttmp, label %while_body_66, label %while_exit_67

while_body_66:                                    ; preds = %while_cond_65
  %"%\D8\B91.load" = load i64, ptr %alloca13, align 8
  call void @"\D8\B9\D8\AF\D8\A7\D8\AF.\D8\B2\D8\AF"(i64 %"%\D8\B91.load")
  %"%\D8\AA.load19" = load i64, ptr %alloca18, align 8
  %addtmp = add i64 %"%\D8\AA.load19", 1
  store i64 %addtmp, ptr %alloca18, align 8
  br label %while_cond_65

while_exit_67:                                    ; preds = %while_cond_65
  store i64 0, ptr %alloca18, align 8
  br label %while_cond_68

while_cond_68:                                    ; preds = %while_body_69, %while_exit_67
  %"%\D8\AA.load20" = load i64, ptr %alloca18, align 8
  %cmplttmp21 = icmp slt i64 %"%\D8\AA.load20", 5
  br i1 %cmplttmp21, label %while_body_69, label %while_exit_70

while_body_69:                                    ; preds = %while_cond_68
  %"%\D8\B92.load" = load i64, ptr %alloca16, align 8
  call void @"\D8\B9\D8\AF\D8\A7\D8\AF.\D8\B2\D8\AF"(i64 %"%\D8\B92.load")
  %"%\D8\AA.load22" = load i64, ptr %alloca18, align 8
  %addtmp23 = add i64 %"%\D8\AA.load22", 1
  store i64 %addtmp23, ptr %alloca18, align 8
  br label %while_cond_68

while_exit_70:                                    ; preds = %while_cond_68
  %"%\D8\B91.load24" = load i64, ptr %alloca13, align 8
  call void @"\D8\B9\D8\AF\D8\A7\D8\AF.\D8\A7\D8\B9\D8\B1\D8\B6"(i64 %"%\D8\B91.load24")
  %"%\D8\B92.load25" = load i64, ptr %alloca16, align 8
  call void @"\D8\B9\D8\AF\D8\A7\D8\AF.\D8\A7\D8\B9\D8\B1\D8\B6"(i64 %"%\D8\B92.load25")
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.81)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.85)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca26 = alloca i64, align 8
  %alloca27 = alloca i64, align 8
  %"%197.load" = load i64, ptr %alloca26, align 8
  store i64 %"%197.load", ptr %alloca27, align 8
  %"%\D8\B31.load" = load i64, ptr %alloca27, align 8
  call void @"\D8\B3\D8\AC\D9\84_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.\D8\B9\D9\8A\D9\91\D9\86_\D8\AD\D9\82\D9\84"(i64 %"%\D8\B31.load", ptr @str.const.89, ptr @str.const.90)
  %"%\D8\B31.load28" = load i64, ptr %alloca27, align 8
  call void @"\D8\B3\D8\AC\D9\84_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.\D8\B9\D9\8A\D9\91\D9\86_\D8\AD\D9\82\D9\84"(i64 %"%\D8\B31.load28", ptr @str.const.91, ptr @str.const.92)
  %"%\D8\B31.load29" = load i64, ptr %alloca27, align 8
  call void @"\D8\B3\D8\AC\D9\84_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.\D8\B9\D9\8A\D9\91\D9\86_\D8\AD\D9\82\D9\84"(i64 %"%\D8\B31.load29", ptr @str.const.93, ptr @str.const.94)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.95)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\B31.load30" = load i64, ptr %alloca27, align 8
  call void @"\D8\B3\D8\AC\D9\84_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.\D8\A7\D8\B9\D8\B1\D8\B6"(i64 %"%\D8\B31.load30")
  %alloca31 = alloca i64, align 8
  %alloca32 = alloca i64, align 8
  %"%209.load" = load i64, ptr %alloca31, align 8
  store i64 %"%209.load", ptr %alloca32, align 8
  %"%\D8\B32.load" = load i64, ptr %alloca32, align 8
  call void @"\D8\B3\D8\AC\D9\84_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.\D8\B9\D9\8A\D9\91\D9\86_\D8\AD\D9\82\D9\84"(i64 %"%\D8\B32.load", ptr @str.const.99, ptr @str.const.100)
  %"%\D8\B32.load33" = load i64, ptr %alloca32, align 8
  call void @"\D8\B3\D8\AC\D9\84_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.\D8\B9\D9\8A\D9\91\D9\86_\D8\AD\D9\82\D9\84"(i64 %"%\D8\B32.load33", ptr @str.const.101, ptr @str.const.102)
  %"%\D8\B32.load34" = load i64, ptr %alloca32, align 8
  call void @"\D8\B3\D8\AC\D9\84_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.\D8\B9\D9\8A\D9\91\D9\86_\D8\AD\D9\82\D9\84"(i64 %"%\D8\B32.load34", ptr @str.const.103, ptr @str.const.104)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.105)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\B32.load35" = load i64, ptr %alloca32, align 8
  call void @"\D8\B3\D8\AC\D9\84_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.\D8\A7\D8\B9\D8\B1\D8\B6"(i64 %"%\D8\B32.load35")
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.109)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.113)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

declare i32 @printf(ptr, ...)

declare i64 @strlen(ptr)

declare ptr @memcpy(ptr, ptr, i64)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
