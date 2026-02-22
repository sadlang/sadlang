; ModuleID = '04_medium_lifetime_analyzer'
source_filename = "04_medium_lifetime_analyzer"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.ret = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.ret.1 = private unnamed_addr constant [4 x i8] c"'\D8\A3\00", align 1
@str.ret.2 = private unnamed_addr constant [4 x i8] c"'\D8\A8\00", align 1
@str.ret.3 = private unnamed_addr constant [4 x i8] c"'\D8\AC\00", align 1
@str.ret.4 = private unnamed_addr constant [4 x i8] c"'\D8\AF\00", align 1
@str.ret.5 = private unnamed_addr constant [12 x i8] c"'\D9\85\D8\AC\D9\87\D9\88\D9\84\00", align 1
@str.const = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.6 = private unnamed_addr constant [3 x i8] c"  \00", align 1
@str.const.7 = private unnamed_addr constant [3 x i8] c": \00", align 1
@str.const.8 = private unnamed_addr constant [18 x i8] c" [\D9\85\D9\84\D9\83] \D8\B9\D9\85\D8\B1=\00", align 1
@str.const.9 = private unnamed_addr constant [3 x i8] c" [\00", align 1
@str.const.10 = private unnamed_addr constant [2 x i8] c"-\00", align 1
@str.const.11 = private unnamed_addr constant [2 x i8] c"]\00", align 1
@str.const.12 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.14 = private unnamed_addr constant [4 x i8] c": &\00", align 1
@str.const.15 = private unnamed_addr constant [15 x i8] c" [\D9\85\D8\B1\D8\AC\D8\B9 -> \00", align 1
@str.const.16 = private unnamed_addr constant [10 x i8] c"] \D8\B9\D9\85\D8\B1=\00", align 1
@str.const.22 = private unnamed_addr constant [15 x i8] c"  \E2\9C\97 \D8\AE\D8\B7\D8\A3: \00", align 1
@str.const.23 = private unnamed_addr constant [21 x i8] c" (\D9\8A\D9\86\D8\AA\D9\87\D9\8A \D8\B3\D8\B7\D8\B1 \00", align 1
@str.const.24 = private unnamed_addr constant [26 x i8] c") \D9\8A\D8\B9\D9\8A\D8\B4 \D8\A3\D8\B7\D9\88\D9\84 \D9\85\D9\86 \00", align 1
@str.const.26 = private unnamed_addr constant [2 x i8] c")\00", align 1
@str.const.29 = private unnamed_addr constant [7 x i8] c"  \E2\9C\93 \00", align 1
@str.const.30 = private unnamed_addr constant [27 x i8] c" \D9\84\D8\A7 \D9\8A\D8\AA\D8\AC\D8\A7\D9\88\D8\B2 \D8\B9\D9\85\D8\B1 \00", align 1
@str.const.34 = private unnamed_addr constant [19 x i8] c"  \E2\9C\97 \D8\AE\D8\B7\D8\A3 \D9\81\D9\8A \00", align 1
@str.const.36 = private unnamed_addr constant [30 x i8] c" \D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A7\D8\AA \D8\AA\D8\B9\D8\AF\D9\8A\D9\84!\00", align 1
@str.const.39 = private unnamed_addr constant [24 x i8] c"  \E2\9C\93 \D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A7\D8\AA \00", align 1
@str.const.40 = private unnamed_addr constant [18 x i8] c" \D9\85\D8\AA\D9\88\D8\A7\D9\81\D9\82\D8\A9 (\00", align 1
@str.const.41 = private unnamed_addr constant [14 x i8] c" \D9\82\D8\B1\D8\A7\D8\A1\D8\A9, \00", align 1
@str.const.42 = private unnamed_addr constant [13 x i8] c" \D8\AA\D8\B9\D8\AF\D9\8A\D9\84)\00", align 1
@str.const.46 = private unnamed_addr constant [34 x i8] c": \D8\AE\D9\84\D8\B7 \D9\82\D8\B1\D8\A7\D8\A1\D8\A9 \D9\88\D8\AA\D8\B9\D8\AF\D9\8A\D9\84!\00", align 1
@str.const.50 = private unnamed_addr constant [48 x i8] c"=== \D8\AA\D8\AD\D9\84\D9\8A\D9\84 \D8\A7\D9\84\D8\A8\D8\B1\D9\86\D8\A7\D9\85\D8\AC 1: \D8\B5\D8\AD\D9\8A\D8\AD ===\00", align 1
@str.const.54 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.58 = private unnamed_addr constant [14 x i8] c"  \D8\A7\D9\84\D9\83\D9\88\D8\AF:\00", align 1
@str.const.62 = private unnamed_addr constant [45 x i8] c"  \E2\94\82 1: \D9\85\D8\AA\D8\BA\D9\8A\D8\B1 \D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA = [1, 2, 3]\00", align 1
@str.const.66 = private unnamed_addr constant [11 x i8] c"  \E2\94\82 2: {\00", align 1
@str.const.70 = private unnamed_addr constant [47 x i8] c"  \E2\94\82 3:   \D9\85\D8\AA\D8\BA\D9\8A\D8\B1 \D9\85\D8\B1\D8\AC\D8\B9 = &\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA\00", align 1
@str.const.74 = private unnamed_addr constant [30 x i8] c"  \E2\94\82 4:   \D8\A7\D8\B7\D8\A8\D8\B9(\D9\85\D8\B1\D8\AC\D8\B9)\00", align 1
@str.const.78 = private unnamed_addr constant [11 x i8] c"  \E2\94\82 5: }\00", align 1
@str.const.82 = private unnamed_addr constant [32 x i8] c"  \E2\94\82 6: \D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.\D8\A3\D8\B6\D9\81(4)\00", align 1
@str.const.86 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.90 = private unnamed_addr constant [22 x i8] c"  \D8\A7\D9\84\D9\85\D8\AA\D8\BA\D9\8A\D8\B1\D8\A7\D8\AA:\00", align 1
@str.const.94 = private unnamed_addr constant [13 x i8] c"\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA\00", align 1
@str.const.95 = private unnamed_addr constant [13 x i8] c"\D9\85\D8\B5\D9\81\D9\88\D9\81\D8\A9\00", align 1
@str.const.96 = private unnamed_addr constant [9 x i8] c"\D9\85\D8\B1\D8\AC\D8\B9\00", align 1
@str.const.99 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.103 = private unnamed_addr constant [20 x i8] c"  \D8\A7\D9\84\D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA:\00", align 1
@str.const.109 = private unnamed_addr constant [11 x i8] c"\D8\B3\D8\B7\D8\B1 3-5\00", align 1
@str.const.110 = private unnamed_addr constant [33 x i8] c"  \E2\86\90 \D8\A7\D9\84\D8\A8\D8\B1\D9\86\D8\A7\D9\85\D8\AC \D8\B5\D8\AD\D9\8A\D8\AD!\00", align 1
@str.const.114 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.118 = private unnamed_addr constant [53 x i8] c"=== \D8\AA\D8\AD\D9\84\D9\8A\D9\84 \D8\A7\D9\84\D8\A8\D8\B1\D9\86\D8\A7\D9\85\D8\AC 2: \D8\AE\D8\B7\D8\A3 \D8\B9\D9\85\D8\B1 ===\00", align 1
@str.const.122 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.130 = private unnamed_addr constant [29 x i8] c"  \E2\94\82 1: \D9\85\D8\AA\D8\BA\D9\8A\D8\B1 \D9\85\D8\B1\D8\AC\D8\B9\00", align 1
@str.const.138 = private unnamed_addr constant [36 x i8] c"  \E2\94\82 3:   \D9\85\D8\AA\D8\BA\D9\8A\D8\B1 \D9\85\D8\AD\D9\84\D9\8A = 42\00", align 1
@str.const.142 = private unnamed_addr constant [32 x i8] c"  \E2\94\82 4:   \D9\85\D8\B1\D8\AC\D8\B9 = &\D9\85\D8\AD\D9\84\D9\8A\00", align 1
@str.const.146 = private unnamed_addr constant [42 x i8] c"  \E2\94\82 5: } // \D9\85\D8\AD\D9\84\D9\8A \D9\8A\D9\8F\D8\AD\D8\B1\D8\B1 \D9\87\D9\86\D8\A7!\00", align 1
@str.const.150 = private unnamed_addr constant [39 x i8] c"  \E2\94\82 6: \D8\A7\D8\B7\D8\A8\D8\B9(\D9\85\D8\B1\D8\AC\D8\B9) // \D8\AE\D8\B7\D8\A3!\00", align 1
@str.const.154 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.163 = private unnamed_addr constant [7 x i8] c"\D8\B1\D9\82\D9\85\00", align 1
@str.const.164 = private unnamed_addr constant [9 x i8] c"\D9\85\D8\AD\D9\84\D9\8A\00", align 1
@str.const.166 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.176 = private unnamed_addr constant [55 x i8] c"  \E2\86\90 \D8\A7\D9\84\D8\A8\D8\B1\D9\86\D8\A7\D9\85\D8\AC \D8\A8\D9\87 \D8\AE\D8\B7\D8\A3: \D9\85\D8\B1\D8\AC\D8\B9 \D9\85\D8\B9\D9\84\D9\82!\00", align 1
@str.const.180 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.184 = private unnamed_addr constant [61 x i8] c"=== \D8\AA\D8\AD\D9\84\D9\8A\D9\84 \D8\A7\D9\84\D8\A8\D8\B1\D9\86\D8\A7\D9\85\D8\AC 3: \D8\AE\D8\B7\D8\A3 \D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9 ===\00", align 1
@str.const.188 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.196 = private unnamed_addr constant [42 x i8] c"  \E2\94\82 1: \D9\85\D8\AA\D8\BA\D9\8A\D8\B1 \D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA = [1, 2]\00", align 1
@str.const.200 = private unnamed_addr constant [40 x i8] c"  \E2\94\82 2: \D9\85\D8\AA\D8\BA\D9\8A\D8\B1 \D9\851 = &\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA\00", align 1
@str.const.204 = private unnamed_addr constant [56 x i8] c"  \E2\94\82 3: \D9\85\D8\AA\D8\BA\D9\8A\D8\B1 \D9\852 = &mut \D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA  // \D8\AE\D8\B7\D8\A3!\00", align 1
@str.const.208 = private unnamed_addr constant [23 x i8] c"  \E2\94\82 4: \D8\A7\D8\B7\D8\A8\D8\B9(\D9\851)\00", align 1
@str.const.212 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.220 = private unnamed_addr constant [11 x i8] c"\D8\B3\D8\B7\D8\B1 2-3\00", align 1
@str.const.221 = private unnamed_addr constant [68 x i8] c"  \E2\86\90 \D8\A7\D9\84\D8\A8\D8\B1\D9\86\D8\A7\D9\85\D8\AC \D8\A8\D9\87 \D8\AE\D8\B7\D8\A3: \D8\AE\D9\84\D8\B7 \D9\82\D8\B1\D8\A7\D8\A1\D8\A9 \D9\88\D8\AA\D8\B9\D8\AF\D9\8A\D9\84!\00", align 1
@str.const.225 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.229 = private unnamed_addr constant [51 x i8] c"=== \D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \D8\A7\D9\84\D8\AA\D8\AD\D9\84\D9\8A\D9\84 \D8\A7\D9\84\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A ===\00", align 1
@str.const.233 = private unnamed_addr constant [91 x i8] c"\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\00", align 1
@str.const.237 = private unnamed_addr constant [35 x i8] c"  \D8\A7\D9\84\D8\A8\D8\B1\D9\86\D8\A7\D9\85\D8\AC 1: \E2\9C\93 \D8\B5\D8\AD\D9\8A\D8\AD\00", align 1
@str.const.241 = private unnamed_addr constant [60 x i8] c"  \D8\A7\D9\84\D8\A8\D8\B1\D9\86\D8\A7\D9\85\D8\AC 2: \E2\9C\97 \D8\AE\D8\B7\D8\A3 \D8\B9\D9\85\D8\B1 (\D9\85\D8\B1\D8\AC\D8\B9 \D9\85\D8\B9\D9\84\D9\82)\00", align 1
@str.const.245 = private unnamed_addr constant [79 x i8] c"  \D8\A7\D9\84\D8\A8\D8\B1\D9\86\D8\A7\D9\85\D8\AC 3: \E2\9C\97 \D8\AE\D8\B7\D8\A3 \D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9 (\D8\AE\D9\84\D8\B7 \D9\82\D8\B1\D8\A7\D8\A1\D8\A9/\D8\AA\D8\B9\D8\AF\D9\8A\D9\84)\00", align 1
@str.const.253 = private unnamed_addr constant [42 x i8] c"  \D8\A7\D9\84\D9\85\D9\84\D8\AE\D8\B5: 1/3 \D8\A8\D8\B1\D8\A7\D9\85\D8\AC \D8\B5\D8\AD\D9\8A\D8\AD\D8\A9\00", align 1
@str.const.257 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.261 = private unnamed_addr constant [109 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.265 = private unnamed_addr constant [58 x i8] c"\E2\95\91  \D9\85\D8\AD\D9\84\D9\84 \D8\A7\D9\84\D8\A3\D8\B9\D9\85\D8\A7\D8\B1 \D8\A7\D9\84\D8\AB\D8\A7\D8\A8\D8\AA             \E2\95\91\00", align 1
@str.const.269 = private unnamed_addr constant [109 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.273 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.277 = private unnamed_addr constant [19 x i8] c"=== \D8\A7\D9\86\D8\AA\D9\87\D9\89 ===\00", align 1

define ptr @"\D8\B9\D9\85\D8\B1_\D9\84\D9\86\D8\B5"(i64 %"\D8\B9\D9\85\D8\B1") {
entry:
  %"\D8\B9\D9\85\D8\B11" = alloca i64, align 8
  store i64 %"\D8\B9\D9\85\D8\B1", ptr %"\D8\B9\D9\85\D8\B11", align 8
  %loadtmp = load i64, ptr %"\D8\B9\D9\85\D8\B11", align 8
  br label %match.case0.test_1

match.end_0:                                      ; preds = %match.case4.test_9
  ret ptr @str.ret

match.case0.test_1:                               ; preds = %entry
  %cmpeqtmp = icmp eq i64 %loadtmp, 1
  br i1 %cmpeqtmp, label %match.case0.body_2, label %match.case1.test_3

match.case0.body_2:                               ; preds = %match.case0.test_1
  ret ptr @str.ret.1

match.case1.test_3:                               ; preds = %match.case0.test_1
  %cmpeqtmp2 = icmp eq i64 %loadtmp, 2
  br i1 %cmpeqtmp2, label %match.case1.body_4, label %match.case2.test_5

match.case1.body_4:                               ; preds = %match.case1.test_3
  ret ptr @str.ret.2

match.case2.test_5:                               ; preds = %match.case1.test_3
  %cmpeqtmp3 = icmp eq i64 %loadtmp, 3
  br i1 %cmpeqtmp3, label %match.case2.body_6, label %match.case3.test_7

match.case2.body_6:                               ; preds = %match.case2.test_5
  ret ptr @str.ret.3

match.case3.test_7:                               ; preds = %match.case2.test_5
  %cmpeqtmp4 = icmp eq i64 %loadtmp, 4
  br i1 %cmpeqtmp4, label %match.case3.body_8, label %match.case4.test_9

match.case3.body_8:                               ; preds = %match.case3.test_7
  ret ptr @str.ret.4

match.case4.test_9:                               ; preds = %match.case3.test_7
  br i1 true, label %match.case4.body_10, label %match.end_0

match.case4.body_10:                              ; preds = %match.case4.test_9
  ret ptr @str.ret.5
}

define void @"\D9\85\D8\AA\D8\BA\D9\8A\D8\B1_\D9\85\D8\AD\D9\84\D9\84.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85", ptr %"\D8\A7\D9\84\D9\86\D9\88\D8\B9", i64 %"\D8\A7\D9\84\D8\B9\D9\85\D8\B1", i64 %"\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", i64 %"\D8\A7\D9\84\D9\86\D9\87\D8\A7\D9\8A\D8\A9") {
entry:
  %"\D8\A7\D9\84\D9\86\D9\87\D8\A7\D9\8A\D8\A96" = alloca i64, align 8
  store i64 %"\D8\A7\D9\84\D9\86\D9\87\D8\A7\D9\8A\D8\A9", ptr %"\D8\A7\D9\84\D9\86\D9\87\D8\A7\D9\8A\D8\A96", align 8
  %"\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A95" = alloca i64, align 8
  store i64 %"\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", ptr %"\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A95", align 8
  %"\D8\A7\D9\84\D8\B9\D9\85\D8\B14" = alloca i64, align 8
  store i64 %"\D8\A7\D9\84\D8\B9\D9\85\D8\B1", ptr %"\D8\A7\D9\84\D8\B9\D9\85\D8\B14", align 8
  %"\D8\A7\D9\84\D9\86\D9\88\D8\B93" = alloca ptr, align 8
  store ptr %"\D8\A7\D9\84\D9\86\D9\88\D8\B9", ptr %"\D8\A7\D9\84\D9\86\D9\88\D8\B93", align 8
  %"\D8\A7\D9\84\D8\A7\D8\B3\D9\852" = alloca ptr, align 8
  store ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85", ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\852", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca7 = alloca ptr, align 8
  %alloca8 = alloca ptr, align 8
  %alloca9 = alloca i64, align 8
  %alloca10 = alloca i64, align 8
  %alloca11 = alloca i1, align 1
  %alloca12 = alloca ptr, align 8
  store ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\852", ptr %alloca7, align 8
  store ptr %"\D8\A7\D9\84\D9\86\D9\88\D8\B93", ptr %alloca8, align 8
  %"%\D8\A7\D9\84\D8\B9\D9\85\D8\B1.load" = load i64, ptr %"\D8\A7\D9\84\D8\B9\D9\85\D8\B14", align 8
  store i64 %"%\D8\A7\D9\84\D8\B9\D9\85\D8\B1.load", ptr %alloca, align 8
  %"%\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load" = load i64, ptr %"\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A95", align 8
  store i64 %"%\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load", ptr %alloca9, align 8
  %"%\D8\A7\D9\84\D9\86\D9\87\D8\A7\D9\8A\D8\A9.load" = load i64, ptr %"\D8\A7\D9\84\D9\86\D9\87\D8\A7\D9\8A\D8\A96", align 8
  store i64 %"%\D8\A7\D9\84\D9\86\D9\87\D8\A7\D9\8A\D8\A9.load", ptr %alloca10, align 8
  store i64 0, ptr %alloca11, align 8
  store ptr @str.const, ptr %alloca12, align 8
  ret void
}

define void @"\D9\85\D8\AA\D8\BA\D9\8A\D8\B1_\D9\85\D8\AD\D9\84\D9\84.\D8\A7\D8\AC\D8\B9\D9\84_\D9\85\D8\B1\D8\AC\D8\B9"(i64 %self, ptr %"\D8\A7\D9\84\D9\85\D8\A7\D9\84\D9\83") {
entry:
  %"\D8\A7\D9\84\D9\85\D8\A7\D9\84\D9\832" = alloca ptr, align 8
  store ptr %"\D8\A7\D9\84\D9\85\D8\A7\D9\84\D9\83", ptr %"\D8\A7\D9\84\D9\85\D8\A7\D9\84\D9\832", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca3 = alloca ptr, align 8
  %alloca4 = alloca ptr, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  %alloca7 = alloca i1, align 1
  %alloca8 = alloca ptr, align 8
  store i64 0, ptr %alloca7, align 8
  store ptr %"\D8\A7\D9\84\D9\85\D8\A7\D9\84\D9\832", ptr %alloca8, align 8
  ret void
}

define void @"\D9\85\D8\AA\D8\BA\D9\8A\D8\B1_\D9\85\D8\AD\D9\84\D9\84.\D8\A7\D8\B9\D8\B1\D8\B6"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca ptr, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i1, align 1
  %alloca7 = alloca ptr, align 8
  %"%\D8\B9\D9\85\D8\B1_\D9\85\D8\AA\D8\BA\D9\8A\D8\B1.load" = load i64, ptr %alloca, align 8
  %calltmp = call ptr @"\D8\B9\D9\85\D8\B1_\D9\84\D9\86\D8\B5"(i64 %"%\D8\B9\D9\85\D8\B1_\D9\85\D8\AA\D8\BA\D9\8A\D8\B1.load")
  %alloca8 = alloca ptr, align 8
  store ptr %calltmp, ptr %alloca8, align 8
  %"%\D9\85\D9\85\D9\84\D9\88\D9\83.load" = load i1, ptr %alloca6, align 1
  br i1 %"%\D9\85\D9\85\D9\84\D9\88\D9\83.load", label %then_11, label %else_13

then_11:                                          ; preds = %entry
  %len1 = call i64 @strlen(ptr @str.const.6)
  %len2 = call i64 @strlen(ptr %alloca2)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.6, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %alloca2, i64 %copylen2)
  %len19 = call i64 @strlen(ptr %concat.buf)
  %len210 = call i64 @strlen(ptr @str.const.7)
  %totallen11 = add i64 %len19, %len210
  %bufsize12 = add i64 %totallen11, 1
  %concat.buf13 = alloca i8, i64 %bufsize12, align 1
  %2 = call ptr @memcpy(ptr %concat.buf13, ptr %concat.buf, i64 %len19)
  %dest214 = getelementptr i8, ptr %concat.buf13, i64 %len19
  %copylen215 = add i64 %len210, 1
  %3 = call ptr @memcpy(ptr %dest214, ptr @str.const.7, i64 %copylen215)
  %len116 = call i64 @strlen(ptr %concat.buf13)
  %len217 = call i64 @strlen(ptr %alloca3)
  %totallen18 = add i64 %len116, %len217
  %bufsize19 = add i64 %totallen18, 1
  %concat.buf20 = alloca i8, i64 %bufsize19, align 1
  %4 = call ptr @memcpy(ptr %concat.buf20, ptr %concat.buf13, i64 %len116)
  %dest221 = getelementptr i8, ptr %concat.buf20, i64 %len116
  %copylen222 = add i64 %len217, 1
  %5 = call ptr @memcpy(ptr %dest221, ptr %alloca3, i64 %copylen222)
  %len123 = call i64 @strlen(ptr %concat.buf20)
  %len224 = call i64 @strlen(ptr @str.const.8)
  %totallen25 = add i64 %len123, %len224
  %bufsize26 = add i64 %totallen25, 1
  %concat.buf27 = alloca i8, i64 %bufsize26, align 1
  %6 = call ptr @memcpy(ptr %concat.buf27, ptr %concat.buf20, i64 %len123)
  %dest228 = getelementptr i8, ptr %concat.buf27, i64 %len123
  %copylen229 = add i64 %len224, 1
  %7 = call ptr @memcpy(ptr %dest228, ptr @str.const.8, i64 %copylen229)
  %len130 = call i64 @strlen(ptr %concat.buf27)
  %len231 = call i64 @strlen(ptr %alloca8)
  %totallen32 = add i64 %len130, %len231
  %bufsize33 = add i64 %totallen32, 1
  %concat.buf34 = alloca i8, i64 %bufsize33, align 1
  %8 = call ptr @memcpy(ptr %concat.buf34, ptr %concat.buf27, i64 %len130)
  %dest235 = getelementptr i8, ptr %concat.buf34, i64 %len130
  %copylen236 = add i64 %len231, 1
  %9 = call ptr @memcpy(ptr %dest235, ptr %alloca8, i64 %copylen236)
  %len137 = call i64 @strlen(ptr %concat.buf34)
  %len238 = call i64 @strlen(ptr @str.const.9)
  %totallen39 = add i64 %len137, %len238
  %bufsize40 = add i64 %totallen39, 1
  %concat.buf41 = alloca i8, i64 %bufsize40, align 1
  %10 = call ptr @memcpy(ptr %concat.buf41, ptr %concat.buf34, i64 %len137)
  %dest242 = getelementptr i8, ptr %concat.buf41, i64 %len137
  %copylen243 = add i64 %len238, 1
  %11 = call ptr @memcpy(ptr %dest242, ptr @str.const.9, i64 %copylen243)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  br label %merge_12

else_13:                                          ; preds = %entry
  %len144 = call i64 @strlen(ptr @str.const.6)
  %len245 = call i64 @strlen(ptr %alloca2)
  %totallen46 = add i64 %len144, %len245
  %bufsize47 = add i64 %totallen46, 1
  %concat.buf48 = alloca i8, i64 %bufsize47, align 1
  %13 = call ptr @memcpy(ptr %concat.buf48, ptr @str.const.6, i64 %len144)
  %dest249 = getelementptr i8, ptr %concat.buf48, i64 %len144
  %copylen250 = add i64 %len245, 1
  %14 = call ptr @memcpy(ptr %dest249, ptr %alloca2, i64 %copylen250)
  %len151 = call i64 @strlen(ptr %concat.buf48)
  %len252 = call i64 @strlen(ptr @str.const.14)
  %totallen53 = add i64 %len151, %len252
  %bufsize54 = add i64 %totallen53, 1
  %concat.buf55 = alloca i8, i64 %bufsize54, align 1
  %15 = call ptr @memcpy(ptr %concat.buf55, ptr %concat.buf48, i64 %len151)
  %dest256 = getelementptr i8, ptr %concat.buf55, i64 %len151
  %copylen257 = add i64 %len252, 1
  %16 = call ptr @memcpy(ptr %dest256, ptr @str.const.14, i64 %copylen257)
  %len158 = call i64 @strlen(ptr %concat.buf55)
  %len259 = call i64 @strlen(ptr %alloca3)
  %totallen60 = add i64 %len158, %len259
  %bufsize61 = add i64 %totallen60, 1
  %concat.buf62 = alloca i8, i64 %bufsize61, align 1
  %17 = call ptr @memcpy(ptr %concat.buf62, ptr %concat.buf55, i64 %len158)
  %dest263 = getelementptr i8, ptr %concat.buf62, i64 %len158
  %copylen264 = add i64 %len259, 1
  %18 = call ptr @memcpy(ptr %dest263, ptr %alloca3, i64 %copylen264)
  %len165 = call i64 @strlen(ptr %concat.buf62)
  %len266 = call i64 @strlen(ptr @str.const.15)
  %totallen67 = add i64 %len165, %len266
  %bufsize68 = add i64 %totallen67, 1
  %concat.buf69 = alloca i8, i64 %bufsize68, align 1
  %19 = call ptr @memcpy(ptr %concat.buf69, ptr %concat.buf62, i64 %len165)
  %dest270 = getelementptr i8, ptr %concat.buf69, i64 %len165
  %copylen271 = add i64 %len266, 1
  %20 = call ptr @memcpy(ptr %dest270, ptr @str.const.15, i64 %copylen271)
  %len172 = call i64 @strlen(ptr %concat.buf69)
  %len273 = call i64 @strlen(ptr %alloca7)
  %totallen74 = add i64 %len172, %len273
  %bufsize75 = add i64 %totallen74, 1
  %concat.buf76 = alloca i8, i64 %bufsize75, align 1
  %21 = call ptr @memcpy(ptr %concat.buf76, ptr %concat.buf69, i64 %len172)
  %dest277 = getelementptr i8, ptr %concat.buf76, i64 %len172
  %copylen278 = add i64 %len273, 1
  %22 = call ptr @memcpy(ptr %dest277, ptr %alloca7, i64 %copylen278)
  %len179 = call i64 @strlen(ptr %concat.buf76)
  %len280 = call i64 @strlen(ptr @str.const.16)
  %totallen81 = add i64 %len179, %len280
  %bufsize82 = add i64 %totallen81, 1
  %concat.buf83 = alloca i8, i64 %bufsize82, align 1
  %23 = call ptr @memcpy(ptr %concat.buf83, ptr %concat.buf76, i64 %len179)
  %dest284 = getelementptr i8, ptr %concat.buf83, i64 %len179
  %copylen285 = add i64 %len280, 1
  %24 = call ptr @memcpy(ptr %dest284, ptr @str.const.16, i64 %copylen285)
  %len186 = call i64 @strlen(ptr %concat.buf83)
  %len287 = call i64 @strlen(ptr %alloca8)
  %totallen88 = add i64 %len186, %len287
  %bufsize89 = add i64 %totallen88, 1
  %concat.buf90 = alloca i8, i64 %bufsize89, align 1
  %25 = call ptr @memcpy(ptr %concat.buf90, ptr %concat.buf83, i64 %len186)
  %dest291 = getelementptr i8, ptr %concat.buf90, i64 %len186
  %copylen292 = add i64 %len287, 1
  %26 = call ptr @memcpy(ptr %dest291, ptr %alloca8, i64 %copylen292)
  %len193 = call i64 @strlen(ptr %concat.buf90)
  %len294 = call i64 @strlen(ptr @str.const.9)
  %totallen95 = add i64 %len193, %len294
  %bufsize96 = add i64 %totallen95, 1
  %concat.buf97 = alloca i8, i64 %bufsize96, align 1
  %27 = call ptr @memcpy(ptr %concat.buf97, ptr %concat.buf90, i64 %len193)
  %dest298 = getelementptr i8, ptr %concat.buf97, i64 %len193
  %copylen299 = add i64 %len294, 1
  %28 = call ptr @memcpy(ptr %dest298, ptr @str.const.9, i64 %copylen299)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  br label %merge_12

merge_12:                                         ; preds = %else_13, %then_11
  ret void
}

define i1 @"\D9\81\D8\AD\D8\B5_\D8\B9\D9\85\D8\B1_\D9\85\D8\B1\D8\AC\D8\B9"(ptr %"\D8\A7\D8\B3\D9\85_\D9\85\D8\B1\D8\AC\D8\B9", i64 %"\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D9\85\D8\B1\D8\AC\D8\B9", ptr %"\D8\A7\D8\B3\D9\85_\D9\85\D8\A7\D9\84\D9\83", i64 %"\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D9\85\D8\A7\D9\84\D9\83") {
entry:
  %"\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D9\85\D8\A7\D9\84\D9\834" = alloca i64, align 8
  store i64 %"\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D9\85\D8\A7\D9\84\D9\83", ptr %"\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D9\85\D8\A7\D9\84\D9\834", align 8
  %"\D8\A7\D8\B3\D9\85_\D9\85\D8\A7\D9\84\D9\833" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85_\D9\85\D8\A7\D9\84\D9\83", ptr %"\D8\A7\D8\B3\D9\85_\D9\85\D8\A7\D9\84\D9\833", align 8
  %"\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D9\85\D8\B1\D8\AC\D8\B92" = alloca i64, align 8
  store i64 %"\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D9\85\D8\B1\D8\AC\D8\B9", ptr %"\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D9\85\D8\B1\D8\AC\D8\B92", align 8
  %"\D8\A7\D8\B3\D9\85_\D9\85\D8\B1\D8\AC\D8\B91" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85_\D9\85\D8\B1\D8\AC\D8\B9", ptr %"\D8\A7\D8\B3\D9\85_\D9\85\D8\B1\D8\AC\D8\B91", align 8
  %"%\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D9\85\D8\B1\D8\AC\D8\B9.load" = load i64, ptr %"\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D9\85\D8\B1\D8\AC\D8\B92", align 8
  %"%\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D9\85\D8\A7\D9\84\D9\83.load" = load i64, ptr %"\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D9\85\D8\A7\D9\84\D9\834", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D9\85\D8\B1\D8\AC\D8\B9.load", %"%\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D9\85\D8\A7\D9\84\D9\83.load"
  br i1 %cmpgttmp, label %then_14, label %merge_15

then_14:                                          ; preds = %entry
  %len1 = call i64 @strlen(ptr @str.const.22)
  %len2 = call i64 @strlen(ptr %"\D8\A7\D8\B3\D9\85_\D9\85\D8\B1\D8\AC\D8\B91")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.22, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\A7\D8\B3\D9\85_\D9\85\D8\B1\D8\AC\D8\B91", i64 %copylen2)
  %len15 = call i64 @strlen(ptr %concat.buf)
  %len26 = call i64 @strlen(ptr @str.const.23)
  %totallen7 = add i64 %len15, %len26
  %bufsize8 = add i64 %totallen7, 1
  %concat.buf9 = alloca i8, i64 %bufsize8, align 1
  %2 = call ptr @memcpy(ptr %concat.buf9, ptr %concat.buf, i64 %len15)
  %dest210 = getelementptr i8, ptr %concat.buf9, i64 %len15
  %copylen211 = add i64 %len26, 1
  %3 = call ptr @memcpy(ptr %dest210, ptr @str.const.23, i64 %copylen211)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  ret i1 false

merge_15:                                         ; preds = %entry
  %len112 = call i64 @strlen(ptr @str.const.29)
  %len213 = call i64 @strlen(ptr %"\D8\A7\D8\B3\D9\85_\D9\85\D8\B1\D8\AC\D8\B91")
  %totallen14 = add i64 %len112, %len213
  %bufsize15 = add i64 %totallen14, 1
  %concat.buf16 = alloca i8, i64 %bufsize15, align 1
  %5 = call ptr @memcpy(ptr %concat.buf16, ptr @str.const.29, i64 %len112)
  %dest217 = getelementptr i8, ptr %concat.buf16, i64 %len112
  %copylen218 = add i64 %len213, 1
  %6 = call ptr @memcpy(ptr %dest217, ptr %"\D8\A7\D8\B3\D9\85_\D9\85\D8\B1\D8\AC\D8\B91", i64 %copylen218)
  %len119 = call i64 @strlen(ptr %concat.buf16)
  %len220 = call i64 @strlen(ptr @str.const.30)
  %totallen21 = add i64 %len119, %len220
  %bufsize22 = add i64 %totallen21, 1
  %concat.buf23 = alloca i8, i64 %bufsize22, align 1
  %7 = call ptr @memcpy(ptr %concat.buf23, ptr %concat.buf16, i64 %len119)
  %dest224 = getelementptr i8, ptr %concat.buf23, i64 %len119
  %copylen225 = add i64 %len220, 1
  %8 = call ptr @memcpy(ptr %dest224, ptr @str.const.30, i64 %copylen225)
  %len126 = call i64 @strlen(ptr %concat.buf23)
  %len227 = call i64 @strlen(ptr %"\D8\A7\D8\B3\D9\85_\D9\85\D8\A7\D9\84\D9\833")
  %totallen28 = add i64 %len126, %len227
  %bufsize29 = add i64 %totallen28, 1
  %concat.buf30 = alloca i8, i64 %bufsize29, align 1
  %9 = call ptr @memcpy(ptr %concat.buf30, ptr %concat.buf23, i64 %len126)
  %dest231 = getelementptr i8, ptr %concat.buf30, i64 %len126
  %copylen232 = add i64 %len227, 1
  %10 = call ptr @memcpy(ptr %dest231, ptr %"\D8\A7\D8\B3\D9\85_\D9\85\D8\A7\D9\84\D9\833", i64 %copylen232)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf30)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  ret i1 true
}

define i1 @"\D9\81\D8\AD\D8\B5_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D9\85\D8\AA\D9\88\D8\A7\D9\81\D9\82\D8\A9"(i64 %"\D8\B9\D8\AF\D8\AF_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", i64 %"\D8\B9\D8\AF\D8\AF_\D8\AA\D8\B9\D8\AF\D9\8A\D9\84", ptr %"\D8\B3\D9\8A\D8\A7\D9\82") {
entry:
  %"\D8\B3\D9\8A\D8\A7\D9\823" = alloca ptr, align 8
  store ptr %"\D8\B3\D9\8A\D8\A7\D9\82", ptr %"\D8\B3\D9\8A\D8\A7\D9\823", align 8
  %"\D8\B9\D8\AF\D8\AF_\D8\AA\D8\B9\D8\AF\D9\8A\D9\842" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF_\D8\AA\D8\B9\D8\AF\D9\8A\D9\84", ptr %"\D8\B9\D8\AF\D8\AF_\D8\AA\D8\B9\D8\AF\D9\8A\D9\842", align 8
  %"\D8\B9\D8\AF\D8\AF_\D9\82\D8\B1\D8\A7\D8\A1\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", ptr %"\D8\B9\D8\AF\D8\AF_\D9\82\D8\B1\D8\A7\D8\A1\D8\A91", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\AA\D8\B9\D8\AF\D9\8A\D9\84.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\AA\D8\B9\D8\AF\D9\8A\D9\842", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D8\AA\D8\B9\D8\AF\D9\8A\D9\84.load", 1
  br i1 %cmpgttmp, label %then_16, label %merge_17

then_16:                                          ; preds = %entry
  %len1 = call i64 @strlen(ptr @str.const.34)
  %len2 = call i64 @strlen(ptr %"\D8\B3\D9\8A\D8\A7\D9\823")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.34, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\B3\D9\8A\D8\A7\D9\823", i64 %copylen2)
  %len14 = call i64 @strlen(ptr %concat.buf)
  %len25 = call i64 @strlen(ptr @str.const.7)
  %totallen6 = add i64 %len14, %len25
  %bufsize7 = add i64 %totallen6, 1
  %concat.buf8 = alloca i8, i64 %bufsize7, align 1
  %2 = call ptr @memcpy(ptr %concat.buf8, ptr %concat.buf, i64 %len14)
  %dest29 = getelementptr i8, ptr %concat.buf8, i64 %len14
  %copylen210 = add i64 %len25, 1
  %3 = call ptr @memcpy(ptr %dest29, ptr @str.const.7, i64 %copylen210)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  ret i1 false

merge_17:                                         ; preds = %entry
  %"%\D8\B9\D8\AF\D8\AF_\D8\AA\D8\B9\D8\AF\D9\8A\D9\84.load11" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\AA\D8\B9\D8\AF\D9\8A\D9\842", align 8
  %cmpgttmp12 = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D8\AA\D8\B9\D8\AF\D9\8A\D9\84.load11", 0
  br i1 %cmpgttmp12, label %then_18, label %merge_19

then_18:                                          ; preds = %merge_17
  %"%\D8\B9\D8\AF\D8\AF_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D9\82\D8\B1\D8\A7\D8\A1\D8\A91", align 8
  %cmpgttmp13 = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load", 0
  br i1 %cmpgttmp13, label %then_20, label %merge_21

merge_19:                                         ; preds = %merge_21, %merge_17
  %len114 = call i64 @strlen(ptr @str.const.39)
  %len215 = call i64 @strlen(ptr %"\D8\B3\D9\8A\D8\A7\D9\823")
  %totallen16 = add i64 %len114, %len215
  %bufsize17 = add i64 %totallen16, 1
  %concat.buf18 = alloca i8, i64 %bufsize17, align 1
  %5 = call ptr @memcpy(ptr %concat.buf18, ptr @str.const.39, i64 %len114)
  %dest219 = getelementptr i8, ptr %concat.buf18, i64 %len114
  %copylen220 = add i64 %len215, 1
  %6 = call ptr @memcpy(ptr %dest219, ptr %"\D8\B3\D9\8A\D8\A7\D9\823", i64 %copylen220)
  %len121 = call i64 @strlen(ptr %concat.buf18)
  %len222 = call i64 @strlen(ptr @str.const.40)
  %totallen23 = add i64 %len121, %len222
  %bufsize24 = add i64 %totallen23, 1
  %concat.buf25 = alloca i8, i64 %bufsize24, align 1
  %7 = call ptr @memcpy(ptr %concat.buf25, ptr %concat.buf18, i64 %len121)
  %dest226 = getelementptr i8, ptr %concat.buf25, i64 %len121
  %copylen227 = add i64 %len222, 1
  %8 = call ptr @memcpy(ptr %dest226, ptr @str.const.40, i64 %copylen227)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  ret i1 true

then_20:                                          ; preds = %then_18
  %len128 = call i64 @strlen(ptr @str.const.34)
  %len229 = call i64 @strlen(ptr %"\D8\B3\D9\8A\D8\A7\D9\823")
  %totallen30 = add i64 %len128, %len229
  %bufsize31 = add i64 %totallen30, 1
  %concat.buf32 = alloca i8, i64 %bufsize31, align 1
  %10 = call ptr @memcpy(ptr %concat.buf32, ptr @str.const.34, i64 %len128)
  %dest233 = getelementptr i8, ptr %concat.buf32, i64 %len128
  %copylen234 = add i64 %len229, 1
  %11 = call ptr @memcpy(ptr %dest233, ptr %"\D8\B3\D9\8A\D8\A7\D9\823", i64 %copylen234)
  %len135 = call i64 @strlen(ptr %concat.buf32)
  %len236 = call i64 @strlen(ptr @str.const.46)
  %totallen37 = add i64 %len135, %len236
  %bufsize38 = add i64 %totallen37, 1
  %concat.buf39 = alloca i8, i64 %bufsize38, align 1
  %12 = call ptr @memcpy(ptr %concat.buf39, ptr %concat.buf32, i64 %len135)
  %dest240 = getelementptr i8, ptr %concat.buf39, i64 %len135
  %copylen241 = add i64 %len236, 1
  %13 = call ptr @memcpy(ptr %dest240, ptr @str.const.46, i64 %copylen241)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf39)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  ret i1 false

merge_21:                                         ; preds = %then_18
  br label %merge_19
}

define i64 @"\D8\AA\D8\AD\D9\84\D9\8A\D9\84_\D8\A8\D8\B1\D9\86\D8\A7\D9\85\D8\AC_1"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.50)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.54)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.58)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.62)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.66)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.70)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.74)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.78)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.82)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.86)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.90)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %alloca = alloca i64, align 8
  %"%125.load" = load i64, ptr %alloca, align 8
  call void @"\D9\85\D8\AA\D8\BA\D9\8A\D8\B1_\D9\85\D8\AD\D9\84\D9\84.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%125.load", ptr @str.const.94, ptr @str.const.95, i64 1, i64 1, i64 6)
  %alloca1 = alloca i64, align 8
  %"%125.load2" = load i64, ptr %alloca, align 8
  store i64 %"%125.load2", ptr %alloca1, align 8
  %"%\D8\A8.load" = load i64, ptr %alloca1, align 8
  call void @"\D9\85\D8\AA\D8\BA\D9\8A\D8\B1_\D9\85\D8\AD\D9\84\D9\84.\D8\A7\D8\B9\D8\B1\D8\B6"(i64 %"%\D8\A8.load")
  %alloca3 = alloca i64, align 8
  %"%133.load" = load i64, ptr %alloca3, align 8
  call void @"\D9\85\D8\AA\D8\BA\D9\8A\D8\B1_\D9\85\D8\AD\D9\84\D9\84.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%133.load", ptr @str.const.96, ptr @str.const.95, i64 2, i64 3, i64 5)
  %alloca4 = alloca i64, align 8
  %"%133.load5" = load i64, ptr %alloca3, align 8
  store i64 %"%133.load5", ptr %alloca4, align 8
  %"%\D9\85.load" = load i64, ptr %alloca4, align 8
  call void @"\D9\85\D8\AA\D8\BA\D9\8A\D8\B1_\D9\85\D8\AD\D9\84\D9\84.\D8\A7\D8\AC\D8\B9\D9\84_\D9\85\D8\B1\D8\AC\D8\B9"(i64 %"%\D9\85.load", ptr @str.const.94)
  %"%\D9\85.load6" = load i64, ptr %alloca4, align 8
  call void @"\D9\85\D8\AA\D8\BA\D9\8A\D8\B1_\D9\85\D8\AD\D9\84\D9\84.\D8\A7\D8\B9\D8\B1\D8\B6"(i64 %"%\D9\85.load6")
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.99)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.103)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %calltmp = call i1 @"\D9\81\D8\AD\D8\B5_\D8\B9\D9\85\D8\B1_\D9\85\D8\B1\D8\AC\D8\B9"(ptr @str.const.96, i64 5, ptr @str.const.94, i64 6)
  %calltmp7 = call i1 @"\D9\81\D8\AD\D8\B5_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D9\85\D8\AA\D9\88\D8\A7\D9\81\D9\82\D8\A9"(i64 1, i64 0, ptr @str.const.109)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.110)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.114)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  ret i64 0
}

define i64 @"\D8\AA\D8\AD\D9\84\D9\8A\D9\84_\D8\A8\D8\B1\D9\86\D8\A7\D9\85\D8\AC_2"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.118)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.122)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.58)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.130)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.66)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.138)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.142)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.146)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.150)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.154)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.90)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %alloca = alloca i64, align 8
  %"%167.load" = load i64, ptr %alloca, align 8
  call void @"\D9\85\D8\AA\D8\BA\D9\8A\D8\B1_\D9\85\D8\AD\D9\84\D9\84.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%167.load", ptr @str.const.96, ptr @str.const.163, i64 1, i64 1, i64 6)
  %alloca1 = alloca i64, align 8
  %"%167.load2" = load i64, ptr %alloca, align 8
  store i64 %"%167.load2", ptr %alloca1, align 8
  %"%\D9\85.load" = load i64, ptr %alloca1, align 8
  call void @"\D9\85\D8\AA\D8\BA\D9\8A\D8\B1_\D9\85\D8\AD\D9\84\D9\84.\D8\A7\D8\B9\D8\B1\D8\B6"(i64 %"%\D9\85.load")
  %alloca3 = alloca i64, align 8
  %"%175.load" = load i64, ptr %alloca3, align 8
  call void @"\D9\85\D8\AA\D8\BA\D9\8A\D8\B1_\D9\85\D8\AD\D9\84\D9\84.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%175.load", ptr @str.const.164, ptr @str.const.163, i64 2, i64 3, i64 5)
  %alloca4 = alloca i64, align 8
  %"%175.load5" = load i64, ptr %alloca3, align 8
  store i64 %"%175.load5", ptr %alloca4, align 8
  %"%\D9\84.load" = load i64, ptr %alloca4, align 8
  call void @"\D9\85\D8\AA\D8\BA\D9\8A\D8\B1_\D9\85\D8\AD\D9\84\D9\84.\D8\A7\D8\B9\D8\B1\D8\B6"(i64 %"%\D9\84.load")
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.166)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.103)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %calltmp = call i1 @"\D9\81\D8\AD\D8\B5_\D8\B9\D9\85\D8\B1_\D9\85\D8\B1\D8\AC\D8\B9"(ptr @str.const.96, i64 6, ptr @str.const.164, i64 5)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.176)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.180)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  ret i64 0
}

define i64 @"\D8\AA\D8\AD\D9\84\D9\8A\D9\84_\D8\A8\D8\B1\D9\86\D8\A7\D9\85\D8\AC_3"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.184)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.188)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.58)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.196)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.200)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.204)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.208)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.212)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.103)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %calltmp = call i1 @"\D9\81\D8\AD\D8\B5_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D9\85\D8\AA\D9\88\D8\A7\D9\81\D9\82\D8\A9"(i64 1, i64 1, ptr @str.const.220)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.221)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.225)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  ret i64 0
}

define i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D8\AA\D8\AD\D9\84\D9\8A\D9\84"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.229)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.233)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.237)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.241)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.245)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.233)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.253)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.257)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  ret i64 0
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.261)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.265)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.269)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.273)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %calltmp = call i64 @"\D8\AA\D8\AD\D9\84\D9\8A\D9\84_\D8\A8\D8\B1\D9\86\D8\A7\D9\85\D8\AC_1"()
  %calltmp1 = call i64 @"\D8\AA\D8\AD\D9\84\D9\8A\D9\84_\D8\A8\D8\B1\D9\86\D8\A7\D9\85\D8\AC_2"()
  %calltmp2 = call i64 @"\D8\AA\D8\AD\D9\84\D9\8A\D9\84_\D8\A8\D8\B1\D9\86\D8\A7\D9\85\D8\AC_3"()
  %calltmp3 = call i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D8\AA\D8\AD\D9\84\D9\8A\D9\84"()
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.277)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  ret i64 0
}

declare i64 @strlen(ptr)

declare ptr @memcpy(ptr, ptr, i64)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
