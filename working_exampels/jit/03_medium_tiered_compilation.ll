; ModuleID = '03_medium_tiered_compilation'
source_filename = "03_medium_tiered_compilation"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [4 x i8] c"  [\00", align 1
@str.const.1 = private unnamed_addr constant [3 x i8] c"] \00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.2 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.ret = private unnamed_addr constant [23 x i8] c"\D9\85\D9\81\D8\B3\D8\B1 (Interpreter)\00", align 1
@str.ret.4 = private unnamed_addr constant [26 x i8] c"JIT \D8\A3\D8\B3\D8\A7\D8\B3\D9\8A (Baseline)\00", align 1
@str.ret.5 = private unnamed_addr constant [27 x i8] c"JIT \D9\85\D8\AD\D8\B3\D9\91\D9\86 (Optimized)\00", align 1
@str.ret.6 = private unnamed_addr constant [29 x i8] c"AOT \D9\83\D8\A7\D9\85\D9\84 (Ahead-of-Time)\00", align 1
@str.ret.7 = private unnamed_addr constant [18 x i8] c"\D8\BA\D9\8A\D8\B1 \D9\85\D8\B9\D8\B1\D9\88\D9\81\00", align 1
@str.const.8 = private unnamed_addr constant [20 x i8] c"  \E2\94\80\E2\94\80 \D8\AF\D8\A7\D9\84\D8\A9: \00", align 1
@str.const.9 = private unnamed_addr constant [8 x i8] c" \E2\94\80\E2\94\80\00", align 1
@str.const.13 = private unnamed_addr constant [34 x i8] c"    \D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A7\D8\AA \D9\85\D8\B7\D9\84\D9\88\D8\A8\D8\A9: \00", align 1
@str.const.16 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.20 = private unnamed_addr constant [33 x i8] c"    \E2\94\80\E2\94\80 \D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9 \E2\94\80\E2\94\80\00", align 1
@str.const.24 = private unnamed_addr constant [25 x i8] c"    \D8\A7\D9\84\D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A7\D8\AA: \00", align 1
@str.const.27 = private unnamed_addr constant [34 x i8] c"    \D8\A7\D9\84\D8\B2\D9\85\D9\86 \D8\A7\D9\84\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A: \00", align 1
@str.const.30 = private unnamed_addr constant [36 x i8] c"    \D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A9 \D8\A7\D9\84\D9\86\D9\87\D8\A7\D8\A6\D9\8A\D8\A9: \00", align 1
@str.const.34 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.38 = private unnamed_addr constant [6 x i8] c"    [\00", align 1
@str.const.39 = private unnamed_addr constant [14 x i8] c"] \D8\AA\D9\86\D9\81\D9\8A\D8\B0 \00", align 1
@str.const.40 = private unnamed_addr constant [20 x i8] c" \D8\AF\D9\88\D8\B1\D8\A9 (\D8\B2\D9\85\D9\86: \00", align 1
@str.const.41 = private unnamed_addr constant [2 x i8] c")\00", align 1
@str.const.44 = private unnamed_addr constant [28 x i8] c"    \E2\AC\86 \D8\AA\D8\B1\D9\82\D9\8A\D8\A9 \D8\A5\D9\84\D9\89: \00", align 1
@str.const.45 = private unnamed_addr constant [24 x i8] c" (\D8\B2\D9\85\D9\86 \D8\A7\D9\84\D8\A8\D9\86\D8\A7\D8\A1: \00", align 1
@str.const.49 = private unnamed_addr constant [47 x i8] c"=== \D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A9 \D8\A3\D9\88\D8\B6\D8\A7\D8\B9 \D8\A7\D9\84\D8\AA\D9\86\D9\81\D9\8A\D8\B0 ===\00", align 1
@str.const.53 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.57 = private unnamed_addr constant [8 x i8] c"  \D9\84\D9\80 \00", align 1
@str.const.58 = private unnamed_addr constant [13 x i8] c" \D8\B9\D9\85\D9\84\D9\8A\D8\A9:\00", align 1
@str.const.61 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.65 = private unnamed_addr constant [3 x i8] c"  \00", align 1
@str.const.69 = private unnamed_addr constant [19 x i8] c"    \D8\A7\D9\84\D8\B3\D8\B1\D8\B9\D8\A9: \00", align 1
@str.const.70 = private unnamed_addr constant [2 x i8] c"x\00", align 1
@str.const.73 = private unnamed_addr constant [28 x i8] c"    \D8\B2\D9\85\D9\86 \D8\A7\D9\84\D8\AA\D9\86\D9\81\D9\8A\D8\B0: \00", align 1
@str.const.76 = private unnamed_addr constant [26 x i8] c"    \D8\B2\D9\85\D9\86 \D8\A7\D9\84\D8\A8\D9\86\D8\A7\D8\A1: \00", align 1
@str.const.79 = private unnamed_addr constant [23 x i8] c"    \D8\A7\D9\84\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A: \00", align 1
@str.const.82 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.86 = private unnamed_addr constant [49 x i8] c"=== \D9\85\D9\84\D8\AE\D8\B5 \D8\A7\D9\84\D8\AA\D8\B1\D8\AC\D9\85\D8\A9 \D8\A7\D9\84\D9\85\D8\AA\D8\AF\D8\B1\D8\AC\D8\A9 ===\00", align 1
@str.const.90 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.94 = private unnamed_addr constant [86 x i8] c"  \D8\A7\D9\84\D9\85\D8\A8\D8\AF\D8\A3: \D8\A7\D9\84\D8\A8\D8\AF\D8\A1 \D8\A8\D8\AA\D9\81\D8\B3\D9\8A\D8\B1 \D8\B3\D8\B1\D9\8A\D8\B9 \D8\AB\D9\85 \D8\A7\D9\84\D8\AA\D8\AD\D8\B3\D9\8A\D9\86 \D8\AA\D8\AF\D8\B1\D9\8A\D8\AC\D9\8A\D8\A7\D9\8B\00", align 1
@str.const.98 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.102 = private unnamed_addr constant [18 x i8] c"  \D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A7\D8\AA:\00", align 1
@str.const.106 = private unnamed_addr constant [68 x i8] c"    0 \E2\86\92 \D9\85\D9\81\D8\B3\D8\B1:    \D8\A8\D8\AF\D9\88\D9\86 \D8\A8\D9\86\D8\A7\D8\A1\D8\8C \D8\A8\D8\B7\D9\8A\D8\A1 \D9\84\D9\83\D9\86 \D9\81\D9\88\D8\B1\D9\8A\00", align 1
@str.const.110 = private unnamed_addr constant [66 x i8] c"    1 \E2\86\92 JIT \D8\A3\D8\B3\D8\A7\D8\B3\D9\8A: \D8\A8\D9\86\D8\A7\D8\A1 \D8\B3\D8\B1\D9\8A\D8\B9\D8\8C \D8\AA\D8\AD\D8\B3\D9\8A\D9\86 \D8\A8\D8\B3\D9\8A\D8\B7\00", align 1
@str.const.114 = private unnamed_addr constant [64 x i8] c"    2 \E2\86\92 JIT \D9\85\D8\AD\D8\B3\D9\91\D9\86: \D8\A8\D9\86\D8\A7\D8\A1 \D8\A3\D8\A8\D8\B7\D8\A3\D8\8C \D8\A3\D8\AF\D8\A7\D8\A1 \D8\A3\D9\81\D8\B6\D9\84\00", align 1
@str.const.118 = private unnamed_addr constant [62 x i8] c"    3 \E2\86\92 AOT \D9\83\D8\A7\D9\85\D9\84: \D8\A8\D9\86\D8\A7\D8\A1 \D8\B7\D9\88\D9\8A\D9\84\D8\8C \D8\A3\D8\AF\D8\A7\D8\A1 \D8\A3\D9\85\D8\AB\D9\84\00", align 1
@str.const.122 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.126 = private unnamed_addr constant [28 x i8] c"  \D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D8\B1\D8\A7\D8\AA\D9\8A\D8\AC\D9\8A\D8\A9:\00", align 1
@str.const.130 = private unnamed_addr constant [56 x i8] c"    - \D8\A7\D9\84\D8\A8\D8\AF\D8\A1 \D8\A8\D8\A7\D9\84\D9\85\D9\81\D8\B3\D8\B1 \D9\84\D8\AC\D9\85\D9\8A\D8\B9 \D8\A7\D9\84\D8\AF\D9\88\D8\A7\D9\84\00", align 1
@str.const.134 = private unnamed_addr constant [45 x i8] c"    - \D9\85\D8\B1\D8\A7\D9\82\D8\A8\D8\A9 \D8\AA\D9\83\D8\B1\D8\A7\D8\B1 \D8\A7\D9\84\D8\AA\D9\86\D9\81\D9\8A\D8\B0\00", align 1
@str.const.138 = private unnamed_addr constant [62 x i8] c"    - \D8\AA\D8\B1\D9\82\D9\8A\D8\A9 \D8\A7\D9\84\D8\AF\D9\88\D8\A7\D9\84 \D8\A7\D9\84\D8\B3\D8\A7\D8\AE\D9\86\D8\A9 \D8\AA\D8\AF\D8\B1\D9\8A\D8\AC\D9\8A\D8\A7\D9\8B\00", align 1
@str.const.142 = private unnamed_addr constant [61 x i8] c"    - \D8\A7\D9\84\D8\AF\D9\88\D8\A7\D9\84 \D8\A7\D9\84\D8\A8\D8\A7\D8\B1\D8\AF\D8\A9 \D8\AA\D8\A8\D9\82\D9\89 \D9\81\D9\8A \D8\A7\D9\84\D9\85\D9\81\D8\B3\D8\B1\00", align 1
@str.const.146 = private unnamed_addr constant [121 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.150 = private unnamed_addr constant [65 x i8] c"\E2\95\91  \D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9 \D8\A7\D9\84\D8\AA\D8\B1\D8\AC\D9\85\D8\A9 \D8\A7\D9\84\D9\85\D8\AA\D8\AF\D8\B1\D8\AC\D8\A9            \E2\95\91\00", align 1
@str.const.154 = private unnamed_addr constant [121 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.158 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.162 = private unnamed_addr constant [45 x i8] c"=== \D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9 \D8\AA\D9\86\D9\81\D9\8A\D8\B0 \D8\A7\D9\84\D8\AF\D9\88\D8\A7\D9\84 ===\00", align 1
@str.const.166 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.170 = private unnamed_addr constant [20 x i8] c"\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9_\D8\B7\D9\84\D8\A8\00", align 1
@str.const.171 = private unnamed_addr constant [20 x i8] c"\D8\AA\D9\87\D9\8A\D8\A6\D8\A9_\D9\86\D8\B8\D8\A7\D9\85\00", align 1
@str.const.172 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.176 = private unnamed_addr constant [36 x i8] c"=== \D8\A7\D9\86\D8\AA\D9\87\D8\AA \D8\A7\D9\84\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9 ===\00", align 1

define i64 @"\D8\B3\D8\AC_\D8\AF\D8\B1\D8\AC\D8\A9"(ptr %"\D8\AF\D8\B1\D8\AC\D8\A9", ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\84") {
entry:
  %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\842" = alloca ptr, align 8
  store ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\84", ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\842", align 8
  %"\D8\AF\D8\B1\D8\AC\D8\A91" = alloca ptr, align 8
  store ptr %"\D8\AF\D8\B1\D8\AC\D8\A9", ptr %"\D8\AF\D8\B1\D8\AC\D8\A91", align 8
  %len1 = call i64 @strlen(ptr @str.const)
  %len2 = call i64 @strlen(ptr %"\D8\AF\D8\B1\D8\AC\D8\A91")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\AF\D8\B1\D8\AC\D8\A91", i64 %copylen2)
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

define ptr @"\D8\A7\D8\B3\D9\85_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %"\D8\AF\D8\B1\D8\AC\D8\A9") {
entry:
  %"\D8\AF\D8\B1\D8\AC\D8\A91" = alloca i64, align 8
  store i64 %"\D8\AF\D8\B1\D8\AC\D8\A9", ptr %"\D8\AF\D8\B1\D8\AC\D8\A91", align 8
  %"%\D8\AF\D8\B1\D8\AC\D8\A9.load" = load i64, ptr %"\D8\AF\D8\B1\D8\AC\D8\A91", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_0, label %merge_1

then_0:                                           ; preds = %entry
  ret ptr @str.ret

merge_1:                                          ; preds = %entry
  %"%\D8\AF\D8\B1\D8\AC\D8\A9.load2" = load i64, ptr %"\D8\AF\D8\B1\D8\AC\D8\A91", align 8
  %cmpeqtmp3 = icmp eq i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9.load2", 1
  br i1 %cmpeqtmp3, label %then_2, label %merge_3

then_2:                                           ; preds = %merge_1
  ret ptr @str.ret.4

merge_3:                                          ; preds = %merge_1
  %"%\D8\AF\D8\B1\D8\AC\D8\A9.load4" = load i64, ptr %"\D8\AF\D8\B1\D8\AC\D8\A91", align 8
  %cmpeqtmp5 = icmp eq i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9.load4", 2
  br i1 %cmpeqtmp5, label %then_4, label %merge_5

then_4:                                           ; preds = %merge_3
  ret ptr @str.ret.5

merge_5:                                          ; preds = %merge_3
  %"%\D8\AF\D8\B1\D8\AC\D8\A9.load6" = load i64, ptr %"\D8\AF\D8\B1\D8\AC\D8\A91", align 8
  %cmpeqtmp7 = icmp eq i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9.load6", 3
  br i1 %cmpeqtmp7, label %then_6, label %merge_7

then_6:                                           ; preds = %merge_5
  ret ptr @str.ret.6

merge_7:                                          ; preds = %merge_5
  ret ptr @str.ret.7
}

define i64 @"\D8\AD\D8\B3\D8\A7\D8\A8_\D8\B3\D8\B1\D8\B9\D8\A9"(i64 %"\D8\AF\D8\B1\D8\AC\D8\A9") {
entry:
  %"\D8\AF\D8\B1\D8\AC\D8\A91" = alloca i64, align 8
  store i64 %"\D8\AF\D8\B1\D8\AC\D8\A9", ptr %"\D8\AF\D8\B1\D8\AC\D8\A91", align 8
  %"%\D8\AF\D8\B1\D8\AC\D8\A9.load" = load i64, ptr %"\D8\AF\D8\B1\D8\AC\D8\A91", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_8, label %merge_9

then_8:                                           ; preds = %entry
  ret i64 1

merge_9:                                          ; preds = %entry
  %"%\D8\AF\D8\B1\D8\AC\D8\A9.load2" = load i64, ptr %"\D8\AF\D8\B1\D8\AC\D8\A91", align 8
  %cmpeqtmp3 = icmp eq i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9.load2", 1
  br i1 %cmpeqtmp3, label %then_10, label %merge_11

then_10:                                          ; preds = %merge_9
  ret i64 10

merge_11:                                         ; preds = %merge_9
  %"%\D8\AF\D8\B1\D8\AC\D8\A9.load4" = load i64, ptr %"\D8\AF\D8\B1\D8\AC\D8\A91", align 8
  %cmpeqtmp5 = icmp eq i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9.load4", 2
  br i1 %cmpeqtmp5, label %then_12, label %merge_13

then_12:                                          ; preds = %merge_11
  ret i64 50

merge_13:                                         ; preds = %merge_11
  %"%\D8\AF\D8\B1\D8\AC\D8\A9.load6" = load i64, ptr %"\D8\AF\D8\B1\D8\AC\D8\A91", align 8
  %cmpeqtmp7 = icmp eq i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9.load6", 3
  br i1 %cmpeqtmp7, label %then_14, label %merge_15

then_14:                                          ; preds = %merge_13
  ret i64 100

merge_15:                                         ; preds = %merge_13
  ret i64 1
}

define i64 @"\D8\B2\D9\85\D9\86_\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"\D8\AF\D8\B1\D8\AC\D8\A9") {
entry:
  %"\D8\AF\D8\B1\D8\AC\D8\A91" = alloca i64, align 8
  store i64 %"\D8\AF\D8\B1\D8\AC\D8\A9", ptr %"\D8\AF\D8\B1\D8\AC\D8\A91", align 8
  %"%\D8\AF\D8\B1\D8\AC\D8\A9.load" = load i64, ptr %"\D8\AF\D8\B1\D8\AC\D8\A91", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_16, label %merge_17

then_16:                                          ; preds = %entry
  ret i64 0

merge_17:                                         ; preds = %entry
  %"%\D8\AF\D8\B1\D8\AC\D8\A9.load2" = load i64, ptr %"\D8\AF\D8\B1\D8\AC\D8\A91", align 8
  %cmpeqtmp3 = icmp eq i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9.load2", 1
  br i1 %cmpeqtmp3, label %then_18, label %merge_19

then_18:                                          ; preds = %merge_17
  ret i64 5

merge_19:                                         ; preds = %merge_17
  %"%\D8\AF\D8\B1\D8\AC\D8\A9.load4" = load i64, ptr %"\D8\AF\D8\B1\D8\AC\D8\A91", align 8
  %cmpeqtmp5 = icmp eq i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9.load4", 2
  br i1 %cmpeqtmp5, label %then_20, label %merge_21

then_20:                                          ; preds = %merge_19
  ret i64 50

merge_21:                                         ; preds = %merge_19
  %"%\D8\AF\D8\B1\D8\AC\D8\A9.load6" = load i64, ptr %"\D8\AF\D8\B1\D8\AC\D8\A91", align 8
  %cmpeqtmp7 = icmp eq i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9.load6", 3
  br i1 %cmpeqtmp7, label %then_22, label %merge_23

then_22:                                          ; preds = %merge_21
  ret i64 500

merge_23:                                         ; preds = %merge_21
  ret i64 0
}

define i64 @"\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9_\D8\AF\D8\A7\D9\84\D8\A9"(ptr %"\D8\A7\D8\B3\D9\85", i64 %"\D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A7\D8\AA") {
entry:
  %"\D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A7\D8\AA2" = alloca i64, align 8
  store i64 %"\D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A7\D8\AA", ptr %"\D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A7\D8\AA2", align 8
  %"\D8\A7\D8\B3\D9\851" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85", ptr %"\D8\A7\D8\B3\D9\851", align 8
  %len1 = call i64 @strlen(ptr @str.const.8)
  %len2 = call i64 @strlen(ptr %"\D8\A7\D8\B3\D9\851")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.8, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\A7\D8\B3\D9\851", i64 %copylen2)
  %len13 = call i64 @strlen(ptr %concat.buf)
  %len24 = call i64 @strlen(ptr @str.const.9)
  %totallen5 = add i64 %len13, %len24
  %bufsize6 = add i64 %totallen5, 1
  %concat.buf7 = alloca i8, i64 %bufsize6, align 1
  %2 = call ptr @memcpy(ptr %concat.buf7, ptr %concat.buf, i64 %len13)
  %dest28 = getelementptr i8, ptr %concat.buf7, i64 %len13
  %copylen29 = add i64 %len24, 1
  %3 = call ptr @memcpy(ptr %dest28, ptr @str.const.9, i64 %copylen29)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf7)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.16)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %alloca10 = alloca i64, align 8
  store i64 0, ptr %alloca10, align 8
  %alloca11 = alloca i64, align 8
  store i64 0, ptr %alloca11, align 8
  br label %while_cond_24

while_cond_24:                                    ; preds = %merge_50, %entry
  %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0_\D8\AD\D8\A7\D9\84\D9\8A.load" = load i64, ptr %alloca10, align 8
  %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A7\D8\AA.load" = load i64, ptr %"\D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A7\D8\AA2", align 8
  %cmplttmp = icmp slt i64 %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0_\D8\AD\D8\A7\D9\84\D9\8A.load", %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A7\D8\AA.load"
  br i1 %cmplttmp, label %while_body_25, label %while_exit_26

while_body_25:                                    ; preds = %while_cond_24
  %alloca12 = alloca i64, align 8
  store i64 0, ptr %alloca12, align 8
  %"%\D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load" = load i64, ptr %alloca, align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_27, label %merge_28

while_exit_26:                                    ; preds = %while_cond_24
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.20)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load13" = load i64, ptr %alloca, align 8
  %calltmp = call ptr @"\D8\A7\D8\B3\D9\85_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %"%\D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load13")
  %alloca14 = alloca ptr, align 8
  store ptr %calltmp, ptr %alloca14, align 8
  %len115 = call i64 @strlen(ptr @str.const.30)
  %len216 = call i64 @strlen(ptr %alloca14)
  %totallen17 = add i64 %len115, %len216
  %bufsize18 = add i64 %totallen17, 1
  %concat.buf19 = alloca i8, i64 %bufsize18, align 1
  %13 = call ptr @memcpy(ptr %concat.buf19, ptr @str.const.30, i64 %len115)
  %dest220 = getelementptr i8, ptr %concat.buf19, i64 %len115
  %copylen221 = add i64 %len216, 1
  %14 = call ptr @memcpy(ptr %dest220, ptr %alloca14, i64 %copylen221)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf19)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.34)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0

then_27:                                          ; preds = %while_body_25
  store i64 50, ptr %alloca12, align 8
  br label %merge_28

merge_28:                                         ; preds = %then_27, %while_body_25
  %"%\D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load22" = load i64, ptr %alloca, align 8
  %cmpeqtmp23 = icmp eq i64 %"%\D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load22", 1
  br i1 %cmpeqtmp23, label %then_29, label %merge_30

then_29:                                          ; preds = %merge_28
  store i64 200, ptr %alloca12, align 8
  br label %merge_30

merge_30:                                         ; preds = %then_29, %merge_28
  %"%\D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load24" = load i64, ptr %alloca, align 8
  %cmpeqtmp25 = icmp eq i64 %"%\D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load24", 2
  br i1 %cmpeqtmp25, label %then_31, label %merge_32

then_31:                                          ; preds = %merge_30
  store i64 1000, ptr %alloca12, align 8
  br label %merge_32

merge_32:                                         ; preds = %then_31, %merge_30
  %"%\D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load26" = load i64, ptr %alloca, align 8
  %cmpeqtmp27 = icmp eq i64 %"%\D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load26", 3
  br i1 %cmpeqtmp27, label %then_33, label %merge_34

then_33:                                          ; preds = %merge_32
  store i64 999999, ptr %alloca12, align 8
  br label %merge_34

merge_34:                                         ; preds = %then_33, %merge_32
  %"%\D8\AD\D8\AF_\D8\AA\D8\B1\D9\82\D9\8A\D8\A9.load" = load i64, ptr %alloca12, align 8
  %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0_\D8\AD\D8\A7\D9\84\D9\8A.load28" = load i64, ptr %alloca10, align 8
  %subtmp = sub i64 %"%\D8\AD\D8\AF_\D8\AA\D8\B1\D9\82\D9\8A\D8\A9.load", %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0_\D8\AD\D8\A7\D9\84\D9\8A.load28"
  %alloca29 = alloca i64, align 8
  store i64 %subtmp, ptr %alloca29, align 8
  %"%\D8\AF\D9\81\D8\B9\D8\A9.load" = load i64, ptr %alloca29, align 8
  %cmpletmp = icmp sle i64 %"%\D8\AF\D9\81\D8\B9\D8\A9.load", 0
  br i1 %cmpletmp, label %then_35, label %merge_36

then_35:                                          ; preds = %merge_34
  %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A7\D8\AA.load30" = load i64, ptr %"\D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A7\D8\AA2", align 8
  %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0_\D8\AD\D8\A7\D9\84\D9\8A.load31" = load i64, ptr %alloca10, align 8
  %subtmp32 = sub i64 %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A7\D8\AA.load30", %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0_\D8\AD\D8\A7\D9\84\D9\8A.load31"
  store i64 %subtmp32, ptr %alloca29, align 8
  br label %merge_36

merge_36:                                         ; preds = %then_35, %merge_34
  %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0_\D8\AD\D8\A7\D9\84\D9\8A.load33" = load i64, ptr %alloca10, align 8
  %"%\D8\AF\D9\81\D8\B9\D8\A9.load34" = load i64, ptr %alloca29, align 8
  %addtmp = add i64 %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0_\D8\AD\D8\A7\D9\84\D9\8A.load33", %"%\D8\AF\D9\81\D8\B9\D8\A9.load34"
  %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A7\D8\AA.load35" = load i64, ptr %"\D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A7\D8\AA2", align 8
  %cmpgttmp = icmp sgt i64 %addtmp, %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A7\D8\AA.load35"
  br i1 %cmpgttmp, label %then_37, label %merge_38

then_37:                                          ; preds = %merge_36
  %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A7\D8\AA.load36" = load i64, ptr %"\D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A7\D8\AA2", align 8
  %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0_\D8\AD\D8\A7\D9\84\D9\8A.load37" = load i64, ptr %alloca10, align 8
  %subtmp38 = sub i64 %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A7\D8\AA.load36", %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0_\D8\AD\D8\A7\D9\84\D9\8A.load37"
  store i64 %subtmp38, ptr %alloca29, align 8
  br label %merge_38

merge_38:                                         ; preds = %then_37, %merge_36
  %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0_\D8\AD\D8\A7\D9\84\D9\8A.load39" = load i64, ptr %alloca10, align 8
  %"%\D8\AF\D9\81\D8\B9\D8\A9.load40" = load i64, ptr %alloca29, align 8
  %addtmp41 = add i64 %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0_\D8\AD\D8\A7\D9\84\D9\8A.load39", %"%\D8\AF\D9\81\D8\B9\D8\A9.load40"
  %"%\D8\AD\D8\AF_\D8\AA\D8\B1\D9\82\D9\8A\D8\A9.load42" = load i64, ptr %alloca12, align 8
  %cmpgttmp43 = icmp sgt i64 %addtmp41, %"%\D8\AD\D8\AF_\D8\AA\D8\B1\D9\82\D9\8A\D8\A9.load42"
  br i1 %cmpgttmp43, label %then_39, label %merge_40

then_39:                                          ; preds = %merge_38
  %"%\D8\AD\D8\AF_\D8\AA\D8\B1\D9\82\D9\8A\D8\A9.load44" = load i64, ptr %alloca12, align 8
  %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0_\D8\AD\D8\A7\D9\84\D9\8A.load45" = load i64, ptr %alloca10, align 8
  %subtmp46 = sub i64 %"%\D8\AD\D8\AF_\D8\AA\D8\B1\D9\82\D9\8A\D8\A9.load44", %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0_\D8\AD\D8\A7\D9\84\D9\8A.load45"
  store i64 %subtmp46, ptr %alloca29, align 8
  br label %merge_40

merge_40:                                         ; preds = %then_39, %merge_38
  %"%\D8\AF\D9\81\D8\B9\D8\A9.load47" = load i64, ptr %alloca29, align 8
  %cmpletmp48 = icmp sle i64 %"%\D8\AF\D9\81\D8\B9\D8\A9.load47", 0
  br i1 %cmpletmp48, label %then_41, label %merge_42

then_41:                                          ; preds = %merge_40
  store i64 1, ptr %alloca29, align 8
  br label %merge_42

merge_42:                                         ; preds = %then_41, %merge_40
  %"%\D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load49" = load i64, ptr %alloca, align 8
  %calltmp50 = call i64 @"\D8\AD\D8\B3\D8\A7\D8\A8_\D8\B3\D8\B1\D8\B9\D8\A9"(i64 %"%\D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load49")
  %alloca51 = alloca i64, align 8
  store i64 %calltmp50, ptr %alloca51, align 8
  %"%\D8\AF\D9\81\D8\B9\D8\A9.load52" = load i64, ptr %alloca29, align 8
  %"%\D8\B3\D8\B1\D8\B9\D8\A9.load" = load i64, ptr %alloca51, align 8
  %divtmp = sdiv i64 %"%\D8\AF\D9\81\D8\B9\D8\A9.load52", %"%\D8\B3\D8\B1\D8\B9\D8\A9.load"
  %alloca53 = alloca i64, align 8
  store i64 %divtmp, ptr %alloca53, align 8
  %"%\D8\B2\D9\85\D9\86_\D8\AF\D9\81\D8\B9\D8\A9.load" = load i64, ptr %alloca53, align 8
  %cmplttmp54 = icmp slt i64 %"%\D8\B2\D9\85\D9\86_\D8\AF\D9\81\D8\B9\D8\A9.load", 1
  br i1 %cmplttmp54, label %then_43, label %merge_44

then_43:                                          ; preds = %merge_42
  store i64 1, ptr %alloca53, align 8
  br label %merge_44

merge_44:                                         ; preds = %then_43, %merge_42
  %"%\D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load55" = load i64, ptr %alloca, align 8
  %calltmp56 = call ptr @"\D8\A7\D8\B3\D9\85_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %"%\D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load55")
  %alloca57 = alloca ptr, align 8
  store ptr %calltmp56, ptr %alloca57, align 8
  %len158 = call i64 @strlen(ptr @str.const.38)
  %len259 = call i64 @strlen(ptr %alloca57)
  %totallen60 = add i64 %len158, %len259
  %bufsize61 = add i64 %totallen60, 1
  %concat.buf62 = alloca i8, i64 %bufsize61, align 1
  %19 = call ptr @memcpy(ptr %concat.buf62, ptr @str.const.38, i64 %len158)
  %dest263 = getelementptr i8, ptr %concat.buf62, i64 %len158
  %copylen264 = add i64 %len259, 1
  %20 = call ptr @memcpy(ptr %dest263, ptr %alloca57, i64 %copylen264)
  %len165 = call i64 @strlen(ptr %concat.buf62)
  %len266 = call i64 @strlen(ptr @str.const.39)
  %totallen67 = add i64 %len165, %len266
  %bufsize68 = add i64 %totallen67, 1
  %concat.buf69 = alloca i8, i64 %bufsize68, align 1
  %21 = call ptr @memcpy(ptr %concat.buf69, ptr %concat.buf62, i64 %len165)
  %dest270 = getelementptr i8, ptr %concat.buf69, i64 %len165
  %copylen271 = add i64 %len266, 1
  %22 = call ptr @memcpy(ptr %dest270, ptr @str.const.39, i64 %copylen271)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0_\D8\AD\D8\A7\D9\84\D9\8A.load72" = load i64, ptr %alloca10, align 8
  %"%\D8\AF\D9\81\D8\B9\D8\A9.load73" = load i64, ptr %alloca29, align 8
  %addtmp74 = add i64 %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0_\D8\AD\D8\A7\D9\84\D9\8A.load72", %"%\D8\AF\D9\81\D8\B9\D8\A9.load73"
  store i64 %addtmp74, ptr %alloca10, align 8
  %"%\D8\B2\D9\85\D9\86_\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load" = load i64, ptr %alloca11, align 8
  %"%\D8\B2\D9\85\D9\86_\D8\AF\D9\81\D8\B9\D8\A9.load75" = load i64, ptr %alloca53, align 8
  %addtmp76 = add i64 %"%\D8\B2\D9\85\D9\86_\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load", %"%\D8\B2\D9\85\D9\86_\D8\AF\D9\81\D8\B9\D8\A9.load75"
  store i64 %addtmp76, ptr %alloca11, align 8
  %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0_\D8\AD\D8\A7\D9\84\D9\8A.load77" = load i64, ptr %alloca10, align 8
  %"%\D8\AD\D8\AF_\D8\AA\D8\B1\D9\82\D9\8A\D8\A9.load78" = load i64, ptr %alloca12, align 8
  %cmpgetmp = icmp sge i64 %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0_\D8\AD\D8\A7\D9\84\D9\8A.load77", %"%\D8\AD\D8\AF_\D8\AA\D8\B1\D9\82\D9\8A\D8\A9.load78"
  br i1 %cmpgetmp, label %then_45, label %merge_46

then_45:                                          ; preds = %merge_44
  %"%\D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load79" = load i64, ptr %alloca, align 8
  %cmplttmp80 = icmp slt i64 %"%\D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load79", 3
  br i1 %cmplttmp80, label %then_47, label %merge_48

merge_46:                                         ; preds = %merge_48, %merge_44
  %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0_\D8\AD\D8\A7\D9\84\D9\8A.load81" = load i64, ptr %alloca10, align 8
  %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A7\D8\AA.load82" = load i64, ptr %"\D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A7\D8\AA2", align 8
  %cmpgetmp83 = icmp sge i64 %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0_\D8\AD\D8\A7\D9\84\D9\8A.load81", %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A7\D8\AA.load82"
  br i1 %cmpgetmp83, label %then_49, label %merge_50

then_47:                                          ; preds = %then_45
  %"%\D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load84" = load i64, ptr %alloca, align 8
  %addtmp85 = add i64 %"%\D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load84", 1
  store i64 %addtmp85, ptr %alloca, align 8
  %"%\D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load86" = load i64, ptr %alloca, align 8
  %calltmp87 = call i64 @"\D8\B2\D9\85\D9\86_\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%\D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load86")
  %alloca88 = alloca i64, align 8
  store i64 %calltmp87, ptr %alloca88, align 8
  %"%\D8\B2\D9\85\D9\86_\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load89" = load i64, ptr %alloca11, align 8
  %"%\D8\B2\D9\85\D9\86_\D8\AA.load" = load i64, ptr %alloca88, align 8
  %addtmp90 = add i64 %"%\D8\B2\D9\85\D9\86_\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load89", %"%\D8\B2\D9\85\D9\86_\D8\AA.load"
  store i64 %addtmp90, ptr %alloca11, align 8
  %"%\D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load91" = load i64, ptr %alloca, align 8
  %calltmp92 = call ptr @"\D8\A7\D8\B3\D9\85_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %"%\D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load91")
  %alloca93 = alloca ptr, align 8
  store ptr %calltmp92, ptr %alloca93, align 8
  %len194 = call i64 @strlen(ptr @str.const.44)
  %len295 = call i64 @strlen(ptr %alloca93)
  %totallen96 = add i64 %len194, %len295
  %bufsize97 = add i64 %totallen96, 1
  %concat.buf98 = alloca i8, i64 %bufsize97, align 1
  %24 = call ptr @memcpy(ptr %concat.buf98, ptr @str.const.44, i64 %len194)
  %dest299 = getelementptr i8, ptr %concat.buf98, i64 %len194
  %copylen2100 = add i64 %len295, 1
  %25 = call ptr @memcpy(ptr %dest299, ptr %alloca93, i64 %copylen2100)
  %len1101 = call i64 @strlen(ptr %concat.buf98)
  %len2102 = call i64 @strlen(ptr @str.const.45)
  %totallen103 = add i64 %len1101, %len2102
  %bufsize104 = add i64 %totallen103, 1
  %concat.buf105 = alloca i8, i64 %bufsize104, align 1
  %26 = call ptr @memcpy(ptr %concat.buf105, ptr %concat.buf98, i64 %len1101)
  %dest2106 = getelementptr i8, ptr %concat.buf105, i64 %len1101
  %copylen2107 = add i64 %len2102, 1
  %27 = call ptr @memcpy(ptr %dest2106, ptr @str.const.45, i64 %copylen2107)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  br label %merge_48

merge_48:                                         ; preds = %then_47, %then_45
  br label %merge_46

then_49:                                          ; preds = %merge_46
  %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A7\D8\AA.load108" = load i64, ptr %"\D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A7\D8\AA2", align 8
  store i64 %"%\D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A7\D8\AA.load108", ptr %alloca10, align 8
  br label %merge_50

merge_50:                                         ; preds = %then_49, %merge_46
  br label %while_cond_24
}

define i64 @"\D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A9_\D8\A3\D9\88\D8\B6\D8\A7\D8\B9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.49)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.53)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca i64, align 8
  store i64 10000, ptr %alloca, align 8
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.61)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca1 = alloca i64, align 8
  store i64 0, ptr %alloca1, align 8
  br label %while_cond_51

while_cond_51:                                    ; preds = %while_body_52, %entry
  %"%\D9\85.load" = load i64, ptr %alloca1, align 8
  %cmpletmp = icmp sle i64 %"%\D9\85.load", 3
  br i1 %cmpletmp, label %while_body_52, label %while_exit_53

while_body_52:                                    ; preds = %while_cond_51
  %"%\D9\85.load2" = load i64, ptr %alloca1, align 8
  %calltmp = call ptr @"\D8\A7\D8\B3\D9\85_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %"%\D9\85.load2")
  %alloca3 = alloca ptr, align 8
  store ptr %calltmp, ptr %alloca3, align 8
  %"%\D9\85.load4" = load i64, ptr %alloca1, align 8
  %calltmp5 = call i64 @"\D8\AD\D8\B3\D8\A7\D8\A8_\D8\B3\D8\B1\D8\B9\D8\A9"(i64 %"%\D9\85.load4")
  %alloca6 = alloca i64, align 8
  store i64 %calltmp5, ptr %alloca6, align 8
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load" = load i64, ptr %alloca, align 8
  %"%\D8\B3\D8\B1\D8\B9\D8\A9_\D9\88.load" = load i64, ptr %alloca6, align 8
  %divtmp = sdiv i64 %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load", %"%\D8\B3\D8\B1\D8\B9\D8\A9_\D9\88.load"
  %alloca7 = alloca i64, align 8
  store i64 %divtmp, ptr %alloca7, align 8
  %"%\D9\85.load8" = load i64, ptr %alloca1, align 8
  %calltmp9 = call i64 @"\D8\B2\D9\85\D9\86_\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%\D9\85.load8")
  %alloca10 = alloca i64, align 8
  store i64 %calltmp9, ptr %alloca10, align 8
  %"%\D8\B2\D9\85\D9\86_\D9\88.load" = load i64, ptr %alloca7, align 8
  %"%\D8\B2\D9\85\D9\86_\D8\A8.load" = load i64, ptr %alloca10, align 8
  %addtmp = add i64 %"%\D8\B2\D9\85\D9\86_\D9\88.load", %"%\D8\B2\D9\85\D9\86_\D8\A8.load"
  %alloca11 = alloca i64, align 8
  store i64 %addtmp, ptr %alloca11, align 8
  %len1 = call i64 @strlen(ptr @str.const.65)
  %len2 = call i64 @strlen(ptr %alloca3)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %7 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.65, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %8 = call ptr @memcpy(ptr %dest2, ptr %alloca3, i64 %copylen2)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.82)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D9\85.load12" = load i64, ptr %alloca1, align 8
  %addtmp13 = add i64 %"%\D9\85.load12", 1
  store i64 %addtmp13, ptr %alloca1, align 8
  br label %while_cond_51

while_exit_53:                                    ; preds = %while_cond_51
  ret i64 0
}

define i64 @"\D9\85\D9\84\D8\AE\D8\B5_\D9\85\D8\AA\D8\AF\D8\B1\D8\AC"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.86)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.90)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.94)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.98)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.102)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.106)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.110)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.114)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.118)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.122)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.126)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.130)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.134)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.138)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.142)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.146)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.150)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.154)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.158)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.162)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.166)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp = call i64 @"\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9_\D8\AF\D8\A7\D9\84\D8\A9"(ptr @str.const.170, i64 300)
  %calltmp1 = call i64 @"\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9_\D8\AF\D8\A7\D9\84\D8\A9"(ptr @str.const.171, i64 5)
  %calltmp2 = call i64 @"\D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A9_\D8\A3\D9\88\D8\B6\D8\A7\D8\B9"()
  %calltmp3 = call i64 @"\D9\85\D9\84\D8\AE\D8\B5_\D9\85\D8\AA\D8\AF\D8\B1\D8\AC"()
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.172)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.176)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
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
