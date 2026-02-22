; ModuleID = '02_simple_hot_path_detector'
source_filename = "02_simple_hot_path_detector"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [12 x i8] c"  [\D9\83\D8\B4\D9\81] \00", align 1
@str.const.1 = private unnamed_addr constant [3 x i8] c": \00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.2 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.ret = private unnamed_addr constant [31 x i8] c"\F0\9F\94\A5\F0\9F\94\A5\F0\9F\94\A5 \D8\B3\D8\A7\D8\AE\D9\86 \D8\AC\D8\AF\D8\A7\D9\8B\00", align 1
@str.ret.4 = private unnamed_addr constant [18 x i8] c"\F0\9F\94\A5\F0\9F\94\A5 \D8\B3\D8\A7\D8\AE\D9\86\00", align 1
@str.ret.5 = private unnamed_addr constant [14 x i8] c"\F0\9F\94\A5 \D8\AF\D8\A7\D9\81\D8\A6\00", align 1
@str.ret.6 = private unnamed_addr constant [16 x i8] c"\E2\9D\84\EF\B8\8F \D8\A8\D8\A7\D8\B1\D8\AF\00", align 1
@str.const.7 = private unnamed_addr constant [3 x i8] c"  \00", align 1
@str.const.9 = private unnamed_addr constant [13 x i8] c" \D9\85\D8\B1\D8\A9 \E2\86\92 \00", align 1
@str.const.12 = private unnamed_addr constant [53 x i8] c"=== \D8\AA\D8\AD\D9\84\D9\8A\D9\84 \D8\A7\D9\84\D8\A8\D8\B1\D9\86\D8\A7\D9\85\D8\AC: \D8\AE\D8\A7\D8\AF\D9\85 \D9\88\D9\8A\D8\A8 ===\00", align 1
@str.const.16 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.20 = private unnamed_addr constant [34 x i8] c"  --- \D8\AA\D9\83\D8\B1\D8\A7\D8\B1 \D8\A7\D9\84\D8\AF\D9\88\D8\A7\D9\84 ---\00", align 1
@str.const.24 = private unnamed_addr constant [22 x i8] c"\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9_\D8\B7\D9\84\D8\A8()\00", align 1
@str.const.25 = private unnamed_addr constant [20 x i8] c"\D8\AA\D8\AD\D9\84\D9\8A\D9\84_\D8\B1\D8\A3\D8\B3()\00", align 1
@str.const.26 = private unnamed_addr constant [24 x i8] c"\D8\A7\D8\AA\D8\B5\D8\A7\D9\84_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9()\00", align 1
@str.const.27 = private unnamed_addr constant [22 x i8] c"\D8\AA\D9\87\D9\8A\D8\A6\D8\A9_\D8\AE\D8\A7\D8\AF\D9\85()\00", align 1
@str.const.28 = private unnamed_addr constant [18 x i8] c"\D8\AA\D8\B1\D9\85\D9\8A\D8\B2_json()\00", align 1
@str.const.29 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.33 = private unnamed_addr constant [44 x i8] c"  --- \D8\A7\D9\84\D9\85\D8\B1\D8\B4\D8\AD\D8\A7\D8\AA \D9\84\D8\AA\D8\B1\D8\AC\D9\85\D8\A9 JIT ---\00", align 1
@str.const.38 = private unnamed_addr constant [26 x i8] c"\D9\85\D8\B1\D8\B4\D8\AD \D9\84\D8\AA\D8\B1\D8\AC\D9\85\D8\A9 JIT\00", align 1
@str.const.43 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.47 = private unnamed_addr constant [28 x i8] c"  \D8\B9\D8\AF\D8\AF \D8\A7\D9\84\D9\85\D8\B1\D8\B4\D8\AD\D8\A7\D8\AA: \00", align 1
@str.const.50 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.54 = private unnamed_addr constant [49 x i8] c"=== \D8\AA\D8\AD\D9\84\D9\8A\D9\84 \D8\A7\D9\84\D8\AD\D9\84\D9\82\D8\A7\D8\AA \D8\A7\D9\84\D8\B3\D8\A7\D8\AE\D9\86\D8\A9 ===\00", align 1
@str.const.58 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.62 = private unnamed_addr constant [52 x i8] c"  --- \D8\AD\D9\84\D9\82\D8\A9 1: \D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9 \D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA ---\00", align 1
@str.const.66 = private unnamed_addr constant [21 x i8] c"    \D8\AA\D9\83\D8\B1\D8\A7\D8\B1\D8\A7\D8\AA: \00", align 1
@str.const.67 = private unnamed_addr constant [6 x i8] c" \E2\86\92 \00", align 1
@str.const.70 = private unnamed_addr constant [47 x i8] c"  --- \D8\AD\D9\84\D9\82\D8\A9 2: \D8\A8\D8\AD\D8\AB \D9\81\D9\8A \D9\85\D8\B5\D9\81\D9\88\D9\81\D8\A9 ---\00", align 1
@str.const.78 = private unnamed_addr constant [44 x i8] c"  --- \D8\AD\D9\84\D9\82\D8\A9 3: \D8\AD\D8\B3\D8\A7\D8\A8 \D8\A7\D9\84\D9\85\D8\AD\D8\B1\D9\83 ---\00", align 1
@str.const.86 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.90 = private unnamed_addr constant [38 x i8] c"  --- \D9\82\D8\B1\D8\A7\D8\B1\D8\A7\D8\AA \D8\A7\D9\84\D8\AA\D8\AD\D8\B3\D9\8A\D9\86 ---\00", align 1
@str.const.94 = private unnamed_addr constant [45 x i8] c"    \D8\AD\D9\84\D9\82\D8\A9 1: \D8\A5\D8\A8\D9\82\D8\A7\D8\A1 \D9\81\D9\8A \D8\A7\D9\84\D9\85\D9\81\D8\B3\D8\B1\00", align 1
@str.const.98 = private unnamed_addr constant [42 x i8] c"    \D8\AD\D9\84\D9\82\D8\A9 2: \D8\AA\D8\B1\D8\AC\D9\85\D8\A9 JIT \D8\AF\D8\B1\D8\AC\D8\A9 1\00", align 1
@str.const.102 = private unnamed_addr constant [64 x i8] c"    \D8\AD\D9\84\D9\82\D8\A9 3: \D8\AA\D8\B1\D8\AC\D9\85\D8\A9 JIT \D8\AF\D8\B1\D8\AC\D8\A9 2 (\D8\AA\D8\AD\D8\B3\D9\8A\D9\86 \D9\83\D8\A7\D9\85\D9\84)\00", align 1
@str.const.106 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.110 = private unnamed_addr constant [38 x i8] c"=== \D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA \D8\A7\D9\84\D9\83\D8\A7\D8\B4\D9\81 ===\00", align 1
@str.const.114 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.118 = private unnamed_addr constant [27 x i8] c"  \D8\AD\D8\AF\D9\88\D8\AF \D8\A7\D9\84\D8\AA\D8\B5\D9\86\D9\8A\D9\81:\00", align 1
@str.const.122 = private unnamed_addr constant [38 x i8] c"    \D8\A8\D8\A7\D8\B1\D8\AF:     < 10 \D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A7\D8\AA\00", align 1
@str.const.126 = private unnamed_addr constant [37 x i8] c"    \D8\AF\D8\A7\D9\81\D8\A6:     10-99 \D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A9\00", align 1
@str.const.130 = private unnamed_addr constant [39 x i8] c"    \D8\B3\D8\A7\D8\AE\D9\86:     100-999 \D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A9\00", align 1
@str.const.134 = private unnamed_addr constant [42 x i8] c"    \D8\B3\D8\A7\D8\AE\D9\86 \D8\AC\D8\AF\D8\A7\D9\8B: 1000+ \D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A9\00", align 1
@str.const.138 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.142 = private unnamed_addr constant [22 x i8] c"  \D8\A7\D9\84\D8\A5\D8\AC\D8\B1\D8\A7\D8\A1\D8\A7\D8\AA:\00", align 1
@str.const.146 = private unnamed_addr constant [37 x i8] c"    \D8\A8\D8\A7\D8\B1\D8\AF \E2\86\92 \D8\AA\D9\81\D8\B3\D9\8A\D8\B1 \D8\B9\D8\A7\D8\AF\D9\8A\00", align 1
@str.const.150 = private unnamed_addr constant [43 x i8] c"    \D8\AF\D8\A7\D9\81\D8\A6 \E2\86\92 \D9\85\D8\B1\D8\A7\D9\82\D8\A8\D8\A9 \D9\85\D8\B3\D8\AA\D9\85\D8\B1\D8\A9\00", align 1
@str.const.154 = private unnamed_addr constant [45 x i8] c"    \D8\B3\D8\A7\D8\AE\D9\86 \E2\86\92 \D8\AA\D8\B1\D8\AC\D9\85\D8\A9 JIT \D8\A3\D8\B3\D8\A7\D8\B3\D9\8A\D8\A9\00", align 1
@str.const.158 = private unnamed_addr constant [54 x i8] c"    \D8\B3\D8\A7\D8\AE\D9\86 \D8\AC\D8\AF\D8\A7\D9\8B \E2\86\92 \D8\AA\D8\B1\D8\AC\D9\85\D8\A9 JIT \D9\85\D8\AD\D8\B3\D9\91\D9\86\D8\A9\00", align 1
@str.const.162 = private unnamed_addr constant [121 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.166 = private unnamed_addr constant [63 x i8] c"\E2\95\91  \D9\83\D8\A7\D8\B4\D9\81 \D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1\D8\A7\D8\AA \D8\A7\D9\84\D8\B3\D8\A7\D8\AE\D9\86\D8\A9              \E2\95\91\00", align 1
@str.const.170 = private unnamed_addr constant [121 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.174 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.178 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.182 = private unnamed_addr constant [32 x i8] c"=== \D8\A7\D9\86\D8\AA\D9\87\D9\89 \D8\A7\D9\84\D9\83\D8\A7\D8\B4\D9\81 ===\00", align 1

define i64 @"\D8\B3\D8\AC_\D9\83\D8\B4\D9\81"(ptr %"\D8\B9\D9\86\D8\B5\D8\B1", ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\84") {
entry:
  %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\842" = alloca ptr, align 8
  store ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\84", ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\842", align 8
  %"\D8\B9\D9\86\D8\B5\D8\B11" = alloca ptr, align 8
  store ptr %"\D8\B9\D9\86\D8\B5\D8\B1", ptr %"\D8\B9\D9\86\D8\B5\D8\B11", align 8
  %len1 = call i64 @strlen(ptr @str.const)
  %len2 = call i64 @strlen(ptr %"\D8\B9\D9\86\D8\B5\D8\B11")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\B9\D9\86\D8\B5\D8\B11", i64 %copylen2)
  %len13 = call i64 @strlen(ptr %concat.buf)
  %len24 = call i64 @strlen(ptr @str.const.1)
  %totallen5 = add i64 %len13, %len24
  %bufsize6 = add i64 %totallen5, 1
  %concat.buf7 = alloca i8, i64 %bufsize6, align 1
  %2 = call ptr @memcpy(ptr %concat.buf7, ptr %concat.buf, i64 %len13)
  %dest28 = getelementptr i8, ptr %concat.buf7, i64 %len13
  %copylen29 = add i64 %len24, 1
  %3 = call ptr @memcpy(ptr %dest28, ptr @str.const.1, i64 %copylen29)
  %len110 = call i64 @strlen(ptr %concat.buf7)
  %len211 = call i64 @strlen(ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\842")
  %totallen12 = add i64 %len110, %len211
  %bufsize13 = add i64 %totallen12, 1
  %concat.buf14 = alloca i8, i64 %bufsize13, align 1
  %4 = call ptr @memcpy(ptr %concat.buf14, ptr %concat.buf7, i64 %len110)
  %dest215 = getelementptr i8, ptr %concat.buf14, i64 %len110
  %copylen216 = add i64 %len211, 1
  %5 = call ptr @memcpy(ptr %dest215, ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\842", i64 %copylen216)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf14)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\AA\D8\B5\D9\86\D9\8A\D9\81_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9"(i64 %"\D8\B9\D8\AF\D8\AF_\D8\AA\D9\86\D9\81\D9\8A\D8\B0") {
entry:
  %"\D8\B9\D8\AF\D8\AF_\D8\AA\D9\86\D9\81\D9\8A\D8\B01" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF_\D8\AA\D9\86\D9\81\D9\8A\D8\B0", ptr %"\D8\B9\D8\AF\D8\AF_\D8\AA\D9\86\D9\81\D9\8A\D8\B01", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\AA\D9\86\D9\81\D9\8A\D8\B0.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\AA\D9\86\D9\81\D9\8A\D8\B01", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B9\D8\AF\D8\AF_\D8\AA\D9\86\D9\81\D9\8A\D8\B0.load", 1000
  br i1 %cmpgetmp, label %then_0, label %merge_1

then_0:                                           ; preds = %entry
  ret i64 3

merge_1:                                          ; preds = %entry
  %"%\D8\B9\D8\AF\D8\AF_\D8\AA\D9\86\D9\81\D9\8A\D8\B0.load2" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\AA\D9\86\D9\81\D9\8A\D8\B01", align 8
  %cmpgetmp3 = icmp sge i64 %"%\D8\B9\D8\AF\D8\AF_\D8\AA\D9\86\D9\81\D9\8A\D8\B0.load2", 100
  br i1 %cmpgetmp3, label %then_2, label %merge_3

then_2:                                           ; preds = %merge_1
  ret i64 2

merge_3:                                          ; preds = %merge_1
  %"%\D8\B9\D8\AF\D8\AF_\D8\AA\D9\86\D9\81\D9\8A\D8\B0.load4" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\AA\D9\86\D9\81\D9\8A\D8\B01", align 8
  %cmpgetmp5 = icmp sge i64 %"%\D8\B9\D8\AF\D8\AF_\D8\AA\D9\86\D9\81\D9\8A\D8\B0.load4", 10
  br i1 %cmpgetmp5, label %then_4, label %merge_5

then_4:                                           ; preds = %merge_3
  ret i64 1

merge_5:                                          ; preds = %merge_3
  ret i64 0
}

define ptr @"\D9\88\D8\B5\D9\81_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9"(i64 %"\D8\AF\D8\B1\D8\AC\D8\A9") {
entry:
  %"\D8\AF\D8\B1\D8\AC\D8\A91" = alloca i64, align 8
  store i64 %"\D8\AF\D8\B1\D8\AC\D8\A9", ptr %"\D8\AF\D8\B1\D8\AC\D8\A91", align 8
  %"%\D8\AF\D8\B1\D8\AC\D8\A9.load" = load i64, ptr %"\D8\AF\D8\B1\D8\AC\D8\A91", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9.load", 3
  br i1 %cmpeqtmp, label %then_6, label %merge_7

then_6:                                           ; preds = %entry
  ret ptr @str.ret

merge_7:                                          ; preds = %entry
  %"%\D8\AF\D8\B1\D8\AC\D8\A9.load2" = load i64, ptr %"\D8\AF\D8\B1\D8\AC\D8\A91", align 8
  %cmpeqtmp3 = icmp eq i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9.load2", 2
  br i1 %cmpeqtmp3, label %then_8, label %merge_9

then_8:                                           ; preds = %merge_7
  ret ptr @str.ret.4

merge_9:                                          ; preds = %merge_7
  %"%\D8\AF\D8\B1\D8\AC\D8\A9.load4" = load i64, ptr %"\D8\AF\D8\B1\D8\AC\D8\A91", align 8
  %cmpeqtmp5 = icmp eq i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9.load4", 1
  br i1 %cmpeqtmp5, label %then_10, label %merge_11

then_10:                                          ; preds = %merge_9
  ret ptr @str.ret.5

merge_11:                                         ; preds = %merge_9
  ret ptr @str.ret.6
}

define i64 @"\D8\B9\D8\B1\D8\B6_\D8\AF\D8\A7\D9\84\D8\A9"(ptr %"\D8\A7\D8\B3\D9\85_\D8\A7\D9\84\D8\AF\D8\A7\D9\84\D8\A9", i64 %"\D8\B9\D8\AF\D8\AF", i64 %"\D8\AF\D8\B1\D8\AC\D8\A9") {
entry:
  %"\D8\AF\D8\B1\D8\AC\D8\A93" = alloca i64, align 8
  store i64 %"\D8\AF\D8\B1\D8\AC\D8\A9", ptr %"\D8\AF\D8\B1\D8\AC\D8\A93", align 8
  %"\D8\B9\D8\AF\D8\AF2" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF", ptr %"\D8\B9\D8\AF\D8\AF2", align 8
  %"\D8\A7\D8\B3\D9\85_\D8\A7\D9\84\D8\AF\D8\A7\D9\84\D8\A91" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85_\D8\A7\D9\84\D8\AF\D8\A7\D9\84\D8\A9", ptr %"\D8\A7\D8\B3\D9\85_\D8\A7\D9\84\D8\AF\D8\A7\D9\84\D8\A91", align 8
  %"%\D8\AF\D8\B1\D8\AC\D8\A9.load" = load i64, ptr %"\D8\AF\D8\B1\D8\AC\D8\A93", align 8
  %calltmp = call ptr @"\D9\88\D8\B5\D9\81_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9"(i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9.load")
  %alloca = alloca ptr, align 8
  store ptr %calltmp, ptr %alloca, align 8
  %len1 = call i64 @strlen(ptr @str.const.7)
  %len2 = call i64 @strlen(ptr %"\D8\A7\D8\B3\D9\85_\D8\A7\D9\84\D8\AF\D8\A7\D9\84\D8\A91")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.7, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\A7\D8\B3\D9\85_\D8\A7\D9\84\D8\AF\D8\A7\D9\84\D8\A91", i64 %copylen2)
  %len14 = call i64 @strlen(ptr %concat.buf)
  %len25 = call i64 @strlen(ptr @str.const.1)
  %totallen6 = add i64 %len14, %len25
  %bufsize7 = add i64 %totallen6, 1
  %concat.buf8 = alloca i8, i64 %bufsize7, align 1
  %2 = call ptr @memcpy(ptr %concat.buf8, ptr %concat.buf, i64 %len14)
  %dest29 = getelementptr i8, ptr %concat.buf8, i64 %len14
  %copylen210 = add i64 %len25, 1
  %3 = call ptr @memcpy(ptr %dest29, ptr @str.const.1, i64 %copylen210)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\AA\D8\AD\D9\84\D9\8A\D9\84_\D8\A8\D8\B1\D9\86\D8\A7\D9\85\D8\AC_1"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.16)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.20)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca i64, align 8
  store i64 5000, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 2000, ptr %alloca1, align 8
  %alloca2 = alloca i64, align 8
  store i64 50, ptr %alloca2, align 8
  %alloca3 = alloca i64, align 8
  store i64 3, ptr %alloca3, align 8
  %alloca4 = alloca i64, align 8
  store i64 800, ptr %alloca4, align 8
  %"%\D8\AF1.load" = load i64, ptr %alloca, align 8
  %calltmp = call i64 @"\D8\AA\D8\B5\D9\86\D9\8A\D9\81_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9"(i64 %"%\D8\AF1.load")
  %alloca5 = alloca i64, align 8
  store i64 %calltmp, ptr %alloca5, align 8
  %"%\D8\AF2.load" = load i64, ptr %alloca1, align 8
  %calltmp6 = call i64 @"\D8\AA\D8\B5\D9\86\D9\8A\D9\81_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9"(i64 %"%\D8\AF2.load")
  %alloca7 = alloca i64, align 8
  store i64 %calltmp6, ptr %alloca7, align 8
  %"%\D8\AF3.load" = load i64, ptr %alloca2, align 8
  %calltmp8 = call i64 @"\D8\AA\D8\B5\D9\86\D9\8A\D9\81_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9"(i64 %"%\D8\AF3.load")
  %alloca9 = alloca i64, align 8
  store i64 %calltmp8, ptr %alloca9, align 8
  %"%\D8\AF4.load" = load i64, ptr %alloca3, align 8
  %calltmp10 = call i64 @"\D8\AA\D8\B5\D9\86\D9\8A\D9\81_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9"(i64 %"%\D8\AF4.load")
  %alloca11 = alloca i64, align 8
  store i64 %calltmp10, ptr %alloca11, align 8
  %"%\D8\AF5.load" = load i64, ptr %alloca4, align 8
  %calltmp12 = call i64 @"\D8\AA\D8\B5\D9\86\D9\8A\D9\81_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9"(i64 %"%\D8\AF5.load")
  %alloca13 = alloca i64, align 8
  store i64 %calltmp12, ptr %alloca13, align 8
  %"%\D8\AF1.load14" = load i64, ptr %alloca, align 8
  %"%\D9\851.load" = load i64, ptr %alloca5, align 8
  %calltmp15 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AF\D8\A7\D9\84\D8\A9"(ptr @str.const.24, i64 %"%\D8\AF1.load14", i64 %"%\D9\851.load")
  %"%\D8\AF2.load16" = load i64, ptr %alloca1, align 8
  %"%\D9\852.load" = load i64, ptr %alloca7, align 8
  %calltmp17 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AF\D8\A7\D9\84\D8\A9"(ptr @str.const.25, i64 %"%\D8\AF2.load16", i64 %"%\D9\852.load")
  %"%\D8\AF3.load18" = load i64, ptr %alloca2, align 8
  %"%\D9\853.load" = load i64, ptr %alloca9, align 8
  %calltmp19 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AF\D8\A7\D9\84\D8\A9"(ptr @str.const.26, i64 %"%\D8\AF3.load18", i64 %"%\D9\853.load")
  %"%\D8\AF4.load20" = load i64, ptr %alloca3, align 8
  %"%\D9\854.load" = load i64, ptr %alloca11, align 8
  %calltmp21 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AF\D8\A7\D9\84\D8\A9"(ptr @str.const.27, i64 %"%\D8\AF4.load20", i64 %"%\D9\854.load")
  %"%\D8\AF5.load22" = load i64, ptr %alloca4, align 8
  %"%\D9\855.load" = load i64, ptr %alloca13, align 8
  %calltmp23 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AF\D8\A7\D9\84\D8\A9"(ptr @str.const.28, i64 %"%\D8\AF5.load22", i64 %"%\D9\855.load")
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.29)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.33)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca24 = alloca i64, align 8
  store i64 0, ptr %alloca24, align 8
  %"%\D9\851.load25" = load i64, ptr %alloca5, align 8
  %cmpgetmp = icmp sge i64 %"%\D9\851.load25", 2
  br i1 %cmpgetmp, label %then_12, label %merge_13

then_12:                                          ; preds = %entry
  %calltmp26 = call i64 @"\D8\B3\D8\AC_\D9\83\D8\B4\D9\81"(ptr @str.const.24, ptr @str.const.38)
  %"%\D9\85\D8\B1\D8\B4\D8\AD\D8\A7\D8\AA.load" = load i64, ptr %alloca24, align 8
  %addtmp = add i64 %"%\D9\85\D8\B1\D8\B4\D8\AD\D8\A7\D8\AA.load", 1
  store i64 %addtmp, ptr %alloca24, align 8
  br label %merge_13

merge_13:                                         ; preds = %then_12, %entry
  %"%\D9\852.load27" = load i64, ptr %alloca7, align 8
  %cmpgetmp28 = icmp sge i64 %"%\D9\852.load27", 2
  br i1 %cmpgetmp28, label %then_14, label %merge_15

then_14:                                          ; preds = %merge_13
  %calltmp29 = call i64 @"\D8\B3\D8\AC_\D9\83\D8\B4\D9\81"(ptr @str.const.25, ptr @str.const.38)
  %"%\D9\85\D8\B1\D8\B4\D8\AD\D8\A7\D8\AA.load30" = load i64, ptr %alloca24, align 8
  %addtmp31 = add i64 %"%\D9\85\D8\B1\D8\B4\D8\AD\D8\A7\D8\AA.load30", 1
  store i64 %addtmp31, ptr %alloca24, align 8
  br label %merge_15

merge_15:                                         ; preds = %then_14, %merge_13
  %"%\D9\853.load32" = load i64, ptr %alloca9, align 8
  %cmpgetmp33 = icmp sge i64 %"%\D9\853.load32", 2
  br i1 %cmpgetmp33, label %then_16, label %merge_17

then_16:                                          ; preds = %merge_15
  %"%\D9\85\D8\B1\D8\B4\D8\AD\D8\A7\D8\AA.load34" = load i64, ptr %alloca24, align 8
  %addtmp35 = add i64 %"%\D9\85\D8\B1\D8\B4\D8\AD\D8\A7\D8\AA.load34", 0
  store i64 %addtmp35, ptr %alloca24, align 8
  br label %merge_17

merge_17:                                         ; preds = %then_16, %merge_15
  %"%\D9\855.load36" = load i64, ptr %alloca13, align 8
  %cmpgetmp37 = icmp sge i64 %"%\D9\855.load36", 2
  br i1 %cmpgetmp37, label %then_18, label %merge_19

then_18:                                          ; preds = %merge_17
  %calltmp38 = call i64 @"\D8\B3\D8\AC_\D9\83\D8\B4\D9\81"(ptr @str.const.28, ptr @str.const.38)
  %"%\D9\85\D8\B1\D8\B4\D8\AD\D8\A7\D8\AA.load39" = load i64, ptr %alloca24, align 8
  %addtmp40 = add i64 %"%\D9\85\D8\B1\D8\B4\D8\AD\D8\A7\D8\AA.load39", 1
  store i64 %addtmp40, ptr %alloca24, align 8
  br label %merge_19

merge_19:                                         ; preds = %then_18, %merge_17
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.43)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.50)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\AA\D8\AD\D9\84\D9\8A\D9\84_\D8\AD\D9\84\D9\82\D8\A7\D8\AA"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.54)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.58)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.62)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 0, ptr %alloca1, align 8
  br label %while_cond_20

while_cond_20:                                    ; preds = %while_body_21, %entry
  %"%\D8\B9.load" = load i64, ptr %alloca1, align 8
  %cmplttmp = icmp slt i64 %"%\D8\B9.load", 20
  br i1 %cmplttmp, label %while_body_21, label %while_exit_22

while_body_21:                                    ; preds = %while_cond_20
  %"%\D8\AA\D9\83\D8\B1\D8\A7\D8\B1_1.load" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D8\AA\D9\83\D8\B1\D8\A7\D8\B1_1.load", 1
  store i64 %addtmp, ptr %alloca, align 8
  %"%\D8\B9.load2" = load i64, ptr %alloca1, align 8
  %addtmp3 = add i64 %"%\D8\B9.load2", 1
  store i64 %addtmp3, ptr %alloca1, align 8
  br label %while_cond_20

while_exit_22:                                    ; preds = %while_cond_20
  %"%\D8\AA\D9\83\D8\B1\D8\A7\D8\B1_1.load4" = load i64, ptr %alloca, align 8
  %calltmp = call i64 @"\D8\AA\D8\B5\D9\86\D9\8A\D9\81_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9"(i64 %"%\D8\AA\D9\83\D8\B1\D8\A7\D8\B1_1.load4")
  %alloca5 = alloca i64, align 8
  store i64 %calltmp, ptr %alloca5, align 8
  %"%\D8\AD\D9\851.load" = load i64, ptr %alloca5, align 8
  %calltmp6 = call ptr @"\D9\88\D8\B5\D9\81_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9"(i64 %"%\D8\AD\D9\851.load")
  %alloca7 = alloca ptr, align 8
  store ptr %calltmp6, ptr %alloca7, align 8
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.70)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca8 = alloca i64, align 8
  store i64 0, ptr %alloca8, align 8
  %alloca9 = alloca i64, align 8
  store i64 0, ptr %alloca9, align 8
  br label %while_cond_23

while_cond_23:                                    ; preds = %while_body_24, %while_exit_22
  %"%\D8\A8.load" = load i64, ptr %alloca9, align 8
  %cmplttmp10 = icmp slt i64 %"%\D8\A8.load", 500
  br i1 %cmplttmp10, label %while_body_24, label %while_exit_25

while_body_24:                                    ; preds = %while_cond_23
  %"%\D8\AA\D9\83\D8\B1\D8\A7\D8\B1_2.load" = load i64, ptr %alloca8, align 8
  %addtmp11 = add i64 %"%\D8\AA\D9\83\D8\B1\D8\A7\D8\B1_2.load", 1
  store i64 %addtmp11, ptr %alloca8, align 8
  %"%\D8\A8.load12" = load i64, ptr %alloca9, align 8
  %addtmp13 = add i64 %"%\D8\A8.load12", 1
  store i64 %addtmp13, ptr %alloca9, align 8
  br label %while_cond_23

while_exit_25:                                    ; preds = %while_cond_23
  %"%\D8\AA\D9\83\D8\B1\D8\A7\D8\B1_2.load14" = load i64, ptr %alloca8, align 8
  %calltmp15 = call i64 @"\D8\AA\D8\B5\D9\86\D9\8A\D9\81_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9"(i64 %"%\D8\AA\D9\83\D8\B1\D8\A7\D8\B1_2.load14")
  %alloca16 = alloca i64, align 8
  store i64 %calltmp15, ptr %alloca16, align 8
  %"%\D8\AD\D9\852.load" = load i64, ptr %alloca16, align 8
  %calltmp17 = call ptr @"\D9\88\D8\B5\D9\81_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9"(i64 %"%\D8\AD\D9\852.load")
  %alloca18 = alloca ptr, align 8
  store ptr %calltmp17, ptr %alloca18, align 8
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.78)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca19 = alloca i64, align 8
  store i64 2500, ptr %alloca19, align 8
  %"%\D8\AA\D9\83\D8\B1\D8\A7\D8\B1_3.load" = load i64, ptr %alloca19, align 8
  %calltmp20 = call i64 @"\D8\AA\D8\B5\D9\86\D9\8A\D9\81_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9"(i64 %"%\D8\AA\D9\83\D8\B1\D8\A7\D8\B1_3.load")
  %alloca21 = alloca i64, align 8
  store i64 %calltmp20, ptr %alloca21, align 8
  %"%\D8\AD\D9\853.load" = load i64, ptr %alloca21, align 8
  %calltmp22 = call ptr @"\D9\88\D8\B5\D9\81_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9"(i64 %"%\D8\AD\D9\853.load")
  %alloca23 = alloca ptr, align 8
  store ptr %calltmp22, ptr %alloca23, align 8
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.86)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.90)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D8\AD\D9\851.load24" = load i64, ptr %alloca5, align 8
  %cmplttmp25 = icmp slt i64 %"%\D8\AD\D9\851.load24", 2
  br i1 %cmplttmp25, label %then_26, label %merge_27

then_26:                                          ; preds = %while_exit_25
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.94)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  br label %merge_27

merge_27:                                         ; preds = %then_26, %while_exit_25
  %"%\D8\AD\D9\852.load26" = load i64, ptr %alloca16, align 8
  %cmpgetmp = icmp sge i64 %"%\D8\AD\D9\852.load26", 2
  br i1 %cmpgetmp, label %then_28, label %merge_29

then_28:                                          ; preds = %merge_27
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.98)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  br label %merge_29

merge_29:                                         ; preds = %then_28, %merge_27
  %"%\D8\AD\D9\853.load27" = load i64, ptr %alloca21, align 8
  %cmpgetmp28 = icmp sge i64 %"%\D8\AD\D9\853.load27", 2
  br i1 %cmpgetmp28, label %then_30, label %merge_31

then_30:                                          ; preds = %merge_29
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.102)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  br label %merge_31

merge_31:                                         ; preds = %then_30, %merge_29
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.106)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.110)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.114)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.118)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.122)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.126)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.130)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.134)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.138)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.142)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.146)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.150)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.154)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.158)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.162)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.166)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.170)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.174)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp = call i64 @"\D8\AA\D8\AD\D9\84\D9\8A\D9\84_\D8\A8\D8\B1\D9\86\D8\A7\D9\85\D8\AC_1"()
  %calltmp1 = call i64 @"\D8\AA\D8\AD\D9\84\D9\8A\D9\84_\D8\AD\D9\84\D9\82\D8\A7\D8\AA"()
  %calltmp2 = call i64 @"\D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA"()
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.178)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.182)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
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
