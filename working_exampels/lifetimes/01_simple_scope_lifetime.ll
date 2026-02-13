; ModuleID = '01_simple_scope_lifetime'
source_filename = "01_simple_scope_lifetime"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [23 x i8] c"-> \D8\AF\D8\AE\D9\88\D9\84 \D9\86\D8\B7\D8\A7\D9\82: \00", align 1
@str.const.1 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.ret = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.2 = private unnamed_addr constant [23 x i8] c"<- \D8\AE\D8\B1\D9\88\D8\AC \D9\86\D8\B7\D8\A7\D9\82: \00", align 1
@str.const.3 = private unnamed_addr constant [14 x i8] c" (\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1)\00", align 1
@str.ret.6 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.7 = private unnamed_addr constant [15 x i8] c"  \D9\85\D8\AA\D8\BA\D9\8A\D8\B1 '\00", align 1
@str.const.8 = private unnamed_addr constant [5 x i8] c"' = \00", align 1
@str.const.12 = private unnamed_addr constant [33 x i8] c"    \D8\A7\D9\84\D8\B9\D9\85\D8\B1: \D9\85\D9\86 \D8\A7\D9\84\D8\B3\D8\B7\D8\B1 \00", align 1
@str.const.13 = private unnamed_addr constant [9 x i8] c" \D8\A5\D9\84\D9\89 \00", align 1
@str.const.16 = private unnamed_addr constant [13 x i8] c"  \D9\85\D8\B1\D8\AC\D8\B9 '\00", align 1
@str.const.17 = private unnamed_addr constant [6 x i8] c"' -> \00", align 1
@str.const.21 = private unnamed_addr constant [26 x i8] c"    \D8\B9\D9\85\D8\B1 \D8\A7\D9\84\D9\85\D8\B1\D8\AC\D8\B9: \00", align 1
@str.const.22 = private unnamed_addr constant [4 x i8] c" - \00", align 1
@str.const.25 = private unnamed_addr constant [45 x i8] c"  \D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9 \D9\82\D8\A7\D8\A8\D9\84\D8\A9 \D9\84\D9\84\D8\AA\D8\B9\D8\AF\D9\8A\D9\84: \00", align 1
@str.const.26 = private unnamed_addr constant [5 x i8] c" <- \00", align 1
@str.const.30 = private unnamed_addr constant [41 x i8] c"  \D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9 \D9\84\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9 \D9\81\D9\82\D8\B7: \00", align 1
@str.const.35 = private unnamed_addr constant [30 x i8] c"  [\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1] \D8\A7\D9\84\D9\85\D9\88\D8\B1\D8\AF '\00", align 1
@str.const.36 = private unnamed_addr constant [20 x i8] c"' \D8\AA\D9\85 \D8\AA\D8\AD\D8\B1\D9\8A\D8\B1\D9\87\00", align 1
@str.const.40 = private unnamed_addr constant [44 x i8] c"=== \D8\A7\D9\84\D9\86\D8\B7\D8\A7\D9\82\D8\A7\D8\AA \D8\A7\D9\84\D9\85\D8\AA\D8\AF\D8\A7\D8\AE\D9\84\D8\A9 ===\00", align 1
@str.const.44 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.48 = private unnamed_addr constant [11 x i8] c"\D8\AE\D8\A7\D8\B1\D8\AC\D9\8A\00", align 1
@str.const.49 = private unnamed_addr constant [13 x i8] c"\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA\00", align 1
@str.const.50 = private unnamed_addr constant [12 x i8] c"[1\D8\8C 2\D8\8C 3]\00", align 1
@str.const.51 = private unnamed_addr constant [13 x i8] c"\D8\AF\D8\A7\D8\AE\D9\84\D9\8A_1\00", align 1
@str.const.52 = private unnamed_addr constant [20 x i8] c"\D9\85\D8\B1\D8\AC\D8\B9_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9\00", align 1
@str.const.58 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.62 = private unnamed_addr constant [13 x i8] c"\D8\AF\D8\A7\D8\AE\D9\84\D9\8A_2\00", align 1
@str.const.63 = private unnamed_addr constant [20 x i8] c"\D9\85\D8\B1\D8\AC\D8\B9_\D8\AA\D8\B9\D8\AF\D9\8A\D9\84\00", align 1
@str.const.67 = private unnamed_addr constant [9 x i8] c"\D8\A3\D8\B9\D9\85\D9\82\00", align 1
@str.const.68 = private unnamed_addr constant [9 x i8] c"\D9\85\D8\AD\D9\84\D9\8A\00", align 1
@str.const.69 = private unnamed_addr constant [3 x i8] c"42\00", align 1
@str.const.74 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.80 = private unnamed_addr constant [36 x i8] c"=== \D8\A7\D9\84\D9\85\D9\84\D9\83\D9\8A\D8\A9 \D9\88\D8\A7\D9\84\D9\86\D9\82\D9\84 ===\00", align 1
@str.const.84 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.88 = private unnamed_addr constant [42 x i8] c"  1. \D8\A5\D9\86\D8\B4\D8\A7\D8\A1 \D8\A7\D9\84\D9\85\D8\A7\D9\84\D9\83 \D8\A7\D9\84\D8\A3\D8\B5\D9\84\D9\8A\00", align 1
@str.const.92 = private unnamed_addr constant [14 x i8] c"\D9\86\D8\B5_\D8\A3\D8\B5\D9\84\D9\8A\00", align 1
@str.const.93 = private unnamed_addr constant [26 x i8] c"\D9\85\D8\B1\D8\AD\D8\A8\D8\A7 \D8\A8\D8\A7\D9\84\D8\B9\D8\A7\D9\84\D9\85\00", align 1
@str.const.94 = private unnamed_addr constant [34 x i8] c"  2. \D9\86\D9\82\D9\84 \D8\A7\D9\84\D9\85\D9\84\D9\83\D9\8A\D8\A9 (move)\00", align 1
@str.const.98 = private unnamed_addr constant [36 x i8] c"     \D9\86\D8\B5_\D8\A3\D8\B5\D9\84\D9\8A -> \D9\86\D8\B5_\D8\AC\D8\AF\D9\8A\D8\AF\00", align 1
@str.const.102 = private unnamed_addr constant [63 x i8] c"     \D9\86\D8\B5_\D8\A3\D8\B5\D9\84\D9\8A \D8\A3\D8\B5\D8\A8\D8\AD \D8\BA\D9\8A\D8\B1 \D8\B5\D8\A7\D9\84\D8\AD \D8\A8\D8\B9\D8\AF \D8\A7\D9\84\D9\86\D9\82\D9\84!\00", align 1
@str.const.106 = private unnamed_addr constant [14 x i8] c"\D9\86\D8\B5_\D8\AC\D8\AF\D9\8A\D8\AF\00", align 1
@str.const.108 = private unnamed_addr constant [62 x i8] c"  3. \D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9 \D9\85\D8\A4\D9\82\D8\AA\D8\A9 \D9\85\D9\86 \D8\A7\D9\84\D9\85\D8\A7\D9\84\D9\83 \D8\A7\D9\84\D8\AC\D8\AF\D9\8A\D8\AF\00", align 1
@str.const.113 = private unnamed_addr constant [18 x i8] c"\D9\85\D8\B1\D8\AC\D8\B9_\D9\85\D8\A4\D9\82\D8\AA\00", align 1
@str.const.114 = private unnamed_addr constant [49 x i8] c"     \D8\A7\D9\84\D9\85\D8\B1\D8\AC\D8\B9 \D8\A7\D9\84\D9\85\D8\A4\D9\82\D8\AA \D9\8A\D9\86\D8\AA\D9\87\D9\8A \D9\87\D9\86\D8\A7\00", align 1
@str.const.118 = private unnamed_addr constant [33 x i8] c"  4. \D9\86\D8\B3\D8\AE \D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9 (clone)\00", align 1
@str.const.122 = private unnamed_addr constant [9 x i8] c"\D9\86\D8\B3\D8\AE\D8\A9\00", align 1
@str.const.124 = private unnamed_addr constant [45 x i8] c"     \D9\83\D9\84\D8\A7\D9\87\D9\85\D8\A7 \D8\B5\D8\A7\D9\84\D8\AD \D8\A8\D8\B9\D8\AF \D8\A7\D9\84\D9\86\D8\B3\D8\AE\00", align 1
@str.const.128 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.132 = private unnamed_addr constant [9 x i8] c"  [\E2\9C\93] \00", align 1
@str.const.136 = private unnamed_addr constant [9 x i8] c"  [\E2\9C\97] \00", align 1
@str.const.140 = private unnamed_addr constant [47 x i8] c"=== \D9\82\D9\88\D8\A7\D8\B9\D8\AF \D8\A7\D9\84\D8\B9\D9\85\D8\B1 \D8\A7\D9\84\D8\A3\D8\B3\D8\A7\D8\B3\D9\8A\D8\A9 ===\00", align 1
@str.const.144 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.148 = private unnamed_addr constant [54 x i8] c"\D8\A7\D9\84\D9\85\D8\B1\D8\AC\D8\B9 \D9\84\D8\A7 \D9\8A\D8\B9\D9\8A\D8\B4 \D8\A3\D8\B7\D9\88\D9\84 \D9\85\D9\86 \D8\A7\D9\84\D9\85\D8\A7\D9\84\D9\83\00", align 1
@str.const.149 = private unnamed_addr constant [59 x i8] c"\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9 \D9\88\D8\A7\D8\AD\D8\AF\D8\A9 \D9\82\D8\A7\D8\A8\D9\84\D8\A9 \D9\84\D9\84\D8\AA\D8\B9\D8\AF\D9\8A\D9\84 \D9\81\D9\82\D8\B7\00", align 1
@str.const.150 = private unnamed_addr constant [52 x i8] c"\D8\B9\D8\AF\D8\A9 \D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A7\D8\AA \D9\84\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9 \D9\85\D8\B3\D9\85\D9\88\D8\AD\D8\A9\00", align 1
@str.const.151 = private unnamed_addr constant [41 x i8] c"\D9\84\D8\A7 \D8\AA\D8\B9\D8\AF\D9\8A\D9\84 \D9\85\D8\B9 \D9\82\D8\B1\D8\A7\D8\A1\D8\A9 \D9\86\D8\B4\D8\B7\D8\A9\00", align 1
@str.const.152 = private unnamed_addr constant [33 x i8] c"\D8\A7\D9\84\D9\86\D9\82\D9\84 \D9\8A\D8\A8\D8\B7\D9\84 \D8\A7\D9\84\D9\85\D8\A7\D9\84\D9\83\00", align 1
@str.const.153 = private unnamed_addr constant [40 x i8] c"\D8\A7\D9\84\D9\86\D8\B3\D8\AE \D9\8A\D8\AD\D8\A7\D9\81\D8\B8 \D8\B9\D9\84\D9\89 \D8\A7\D9\84\D8\A3\D8\B5\D9\84\00", align 1
@str.const.154 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.158 = private unnamed_addr constant [19 x i8] c"  \D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9: \00", align 1
@str.const.159 = private unnamed_addr constant [29 x i8] c"/6 \D9\82\D9\88\D8\A7\D8\B9\D8\AF \D9\85\D8\B3\D8\AA\D9\88\D9\81\D8\A7\D8\A9\00", align 1
@str.const.162 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.166 = private unnamed_addr constant [109 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.170 = private unnamed_addr constant [59 x i8] c"\E2\95\91  \D9\86\D8\B7\D8\A7\D9\82\D8\A7\D8\AA \D8\A7\D9\84\D8\B9\D9\85\D8\B1 \D9\88\D8\A7\D9\84\D9\85\D8\B1\D8\A7\D8\AC\D8\B9          \E2\95\91\00", align 1
@str.const.174 = private unnamed_addr constant [109 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.178 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.182 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.186 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.190 = private unnamed_addr constant [36 x i8] c"=== \D8\A7\D9\86\D8\AA\D9\87\D9\89 \D8\A7\D9\84\D8\A8\D8\B1\D9\86\D8\A7\D9\85\D8\AC ===\00", align 1

define ptr @"\D8\AA\D8\B3\D8\AC\D9\8A\D9\84_\D8\AF\D8\AE\D9\88\D9\84"(ptr %"\D8\A7\D8\B3\D9\85_\D9\86\D8\B7\D8\A7\D9\82", i64 %"\D8\B9\D9\85\D9\82") {
entry:
  %"\D8\B9\D9\85\D9\822" = alloca i64, align 8
  store i64 %"\D8\B9\D9\85\D9\82", ptr %"\D8\B9\D9\85\D9\822", align 8
  %"\D8\A7\D8\B3\D9\85_\D9\86\D8\B7\D8\A7\D9\821" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85_\D9\86\D8\B7\D8\A7\D9\82", ptr %"\D8\A7\D8\B3\D9\85_\D9\86\D8\B7\D8\A7\D9\821", align 8
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  br label %while_cond_0

while_cond_0:                                     ; preds = %while_body_1, %entry
  %"%\D8\B9.load" = load i64, ptr %alloca, align 8
  %"%\D8\B9\D9\85\D9\82.load" = load i64, ptr %"\D8\B9\D9\85\D9\822", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B9.load", %"%\D8\B9\D9\85\D9\82.load"
  br i1 %cmplttmp, label %while_body_1, label %while_exit_2

while_body_1:                                     ; preds = %while_cond_0
  %"%\D8\B9.load3" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D8\B9.load3", 1
  store i64 %addtmp, ptr %alloca, align 8
  br label %while_cond_0

while_exit_2:                                     ; preds = %while_cond_0
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret ptr @str.ret
}

define ptr @"\D8\AA\D8\B3\D8\AC\D9\8A\D9\84_\D8\AE\D8\B1\D9\88\D8\AC"(ptr %"\D8\A7\D8\B3\D9\85_\D9\86\D8\B7\D8\A7\D9\82", i64 %"\D8\B9\D9\85\D9\82") {
entry:
  %"\D8\B9\D9\85\D9\822" = alloca i64, align 8
  store i64 %"\D8\B9\D9\85\D9\82", ptr %"\D8\B9\D9\85\D9\822", align 8
  %"\D8\A7\D8\B3\D9\85_\D9\86\D8\B7\D8\A7\D9\821" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85_\D9\86\D8\B7\D8\A7\D9\82", ptr %"\D8\A7\D8\B3\D9\85_\D9\86\D8\B7\D8\A7\D9\821", align 8
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  br label %while_cond_3

while_cond_3:                                     ; preds = %while_body_4, %entry
  %"%\D8\B9.load" = load i64, ptr %alloca, align 8
  %"%\D8\B9\D9\85\D9\82.load" = load i64, ptr %"\D8\B9\D9\85\D9\822", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B9.load", %"%\D8\B9\D9\85\D9\82.load"
  br i1 %cmplttmp, label %while_body_4, label %while_exit_5

while_body_4:                                     ; preds = %while_cond_3
  %"%\D8\B9.load3" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D8\B9.load3", 1
  store i64 %addtmp, ptr %alloca, align 8
  br label %while_cond_3

while_exit_5:                                     ; preds = %while_cond_3
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret ptr @str.ret.6
}

define i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\AA\D8\BA\D9\8A\D8\B1"(ptr %"\D8\A7\D8\B3\D9\85", ptr %"\D9\82\D9\8A\D9\85\D8\A9", i64 %"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", i64 %"\D9\86\D9\87\D8\A7\D9\8A\D8\AA\D9\87") {
entry:
  %"\D9\86\D9\87\D8\A7\D9\8A\D8\AA\D9\874" = alloca i64, align 8
  store i64 %"\D9\86\D9\87\D8\A7\D9\8A\D8\AA\D9\87", ptr %"\D9\86\D9\87\D8\A7\D9\8A\D8\AA\D9\874", align 8
  %"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A93" = alloca i64, align 8
  store i64 %"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", ptr %"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A93", align 8
  %"\D9\82\D9\8A\D9\85\D8\A92" = alloca ptr, align 8
  store ptr %"\D9\82\D9\8A\D9\85\D8\A9", ptr %"\D9\82\D9\8A\D9\85\D8\A92", align 8
  %"\D8\A7\D8\B3\D9\851" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85", ptr %"\D8\A7\D8\B3\D9\851", align 8
  %len1 = call i64 @strlen(ptr @str.const.7)
  %len2 = call i64 @strlen(ptr %"\D8\A7\D8\B3\D9\851")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.7, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\A7\D8\B3\D9\851", i64 %copylen2)
  %len15 = call i64 @strlen(ptr %concat.buf)
  %len26 = call i64 @strlen(ptr @str.const.8)
  %totallen7 = add i64 %len15, %len26
  %bufsize8 = add i64 %totallen7, 1
  %concat.buf9 = alloca i8, i64 %bufsize8, align 1
  %2 = call ptr @memcpy(ptr %concat.buf9, ptr %concat.buf, i64 %len15)
  %dest210 = getelementptr i8, ptr %concat.buf9, i64 %len15
  %copylen211 = add i64 %len26, 1
  %3 = call ptr @memcpy(ptr %dest210, ptr @str.const.8, i64 %copylen211)
  %len112 = call i64 @strlen(ptr %concat.buf9)
  %len213 = call i64 @strlen(ptr %"\D9\82\D9\8A\D9\85\D8\A92")
  %totallen14 = add i64 %len112, %len213
  %bufsize15 = add i64 %totallen14, 1
  %concat.buf16 = alloca i8, i64 %bufsize15, align 1
  %4 = call ptr @memcpy(ptr %concat.buf16, ptr %concat.buf9, i64 %len112)
  %dest217 = getelementptr i8, ptr %concat.buf16, i64 %len112
  %copylen218 = add i64 %len213, 1
  %5 = call ptr @memcpy(ptr %dest217, ptr %"\D9\82\D9\8A\D9\85\D8\A92", i64 %copylen218)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf16)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\B1\D8\AC\D8\B9"(ptr %"\D8\A7\D8\B3\D9\85_\D9\85\D8\B1\D8\AC\D8\B9", ptr %"\D8\A7\D8\B3\D9\85_\D9\85\D8\A7\D9\84\D9\83", i64 %"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", i64 %"\D9\86\D9\87\D8\A7\D9\8A\D8\AA\D9\87") {
entry:
  %"\D9\86\D9\87\D8\A7\D9\8A\D8\AA\D9\874" = alloca i64, align 8
  store i64 %"\D9\86\D9\87\D8\A7\D9\8A\D8\AA\D9\87", ptr %"\D9\86\D9\87\D8\A7\D9\8A\D8\AA\D9\874", align 8
  %"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A93" = alloca i64, align 8
  store i64 %"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", ptr %"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A93", align 8
  %"\D8\A7\D8\B3\D9\85_\D9\85\D8\A7\D9\84\D9\832" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85_\D9\85\D8\A7\D9\84\D9\83", ptr %"\D8\A7\D8\B3\D9\85_\D9\85\D8\A7\D9\84\D9\832", align 8
  %"\D8\A7\D8\B3\D9\85_\D9\85\D8\B1\D8\AC\D8\B91" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85_\D9\85\D8\B1\D8\AC\D8\B9", ptr %"\D8\A7\D8\B3\D9\85_\D9\85\D8\B1\D8\AC\D8\B91", align 8
  %len1 = call i64 @strlen(ptr @str.const.16)
  %len2 = call i64 @strlen(ptr %"\D8\A7\D8\B3\D9\85_\D9\85\D8\B1\D8\AC\D8\B91")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.16, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\A7\D8\B3\D9\85_\D9\85\D8\B1\D8\AC\D8\B91", i64 %copylen2)
  %len15 = call i64 @strlen(ptr %concat.buf)
  %len26 = call i64 @strlen(ptr @str.const.17)
  %totallen7 = add i64 %len15, %len26
  %bufsize8 = add i64 %totallen7, 1
  %concat.buf9 = alloca i8, i64 %bufsize8, align 1
  %2 = call ptr @memcpy(ptr %concat.buf9, ptr %concat.buf, i64 %len15)
  %dest210 = getelementptr i8, ptr %concat.buf9, i64 %len15
  %copylen211 = add i64 %len26, 1
  %3 = call ptr @memcpy(ptr %dest210, ptr @str.const.17, i64 %copylen211)
  %len112 = call i64 @strlen(ptr %concat.buf9)
  %len213 = call i64 @strlen(ptr %"\D8\A7\D8\B3\D9\85_\D9\85\D8\A7\D9\84\D9\832")
  %totallen14 = add i64 %len112, %len213
  %bufsize15 = add i64 %totallen14, 1
  %concat.buf16 = alloca i8, i64 %bufsize15, align 1
  %4 = call ptr @memcpy(ptr %concat.buf16, ptr %concat.buf9, i64 %len112)
  %dest217 = getelementptr i8, ptr %concat.buf16, i64 %len112
  %copylen218 = add i64 %len213, 1
  %5 = call ptr @memcpy(ptr %dest217, ptr %"\D8\A7\D8\B3\D9\85_\D9\85\D8\A7\D9\84\D9\832", i64 %copylen218)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf16)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\AA\D8\AD\D9\82\D9\82_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9"(ptr %"\D9\85\D8\A7\D9\84\D9\83", ptr %"\D9\85\D8\B3\D8\AA\D8\B9\D9\8A\D8\B1", i64 %"\D9\82\D8\A7\D8\A8\D9\84_\D9\84\D9\84\D8\AA\D8\B9\D8\AF\D9\8A\D9\84") {
entry:
  %"\D9\82\D8\A7\D8\A8\D9\84_\D9\84\D9\84\D8\AA\D8\B9\D8\AF\D9\8A\D9\843" = alloca i64, align 8
  store i64 %"\D9\82\D8\A7\D8\A8\D9\84_\D9\84\D9\84\D8\AA\D8\B9\D8\AF\D9\8A\D9\84", ptr %"\D9\82\D8\A7\D8\A8\D9\84_\D9\84\D9\84\D8\AA\D8\B9\D8\AF\D9\8A\D9\843", align 8
  %"\D9\85\D8\B3\D8\AA\D8\B9\D9\8A\D8\B12" = alloca ptr, align 8
  store ptr %"\D9\85\D8\B3\D8\AA\D8\B9\D9\8A\D8\B1", ptr %"\D9\85\D8\B3\D8\AA\D8\B9\D9\8A\D8\B12", align 8
  %"\D9\85\D8\A7\D9\84\D9\831" = alloca ptr, align 8
  store ptr %"\D9\85\D8\A7\D9\84\D9\83", ptr %"\D9\85\D8\A7\D9\84\D9\831", align 8
  %"%\D9\82\D8\A7\D8\A8\D9\84_\D9\84\D9\84\D8\AA\D8\B9\D8\AF\D9\8A\D9\84.load" = load i64, ptr %"\D9\82\D8\A7\D8\A8\D9\84_\D9\84\D9\84\D8\AA\D8\B9\D8\AF\D9\8A\D9\843", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\82\D8\A7\D8\A8\D9\84_\D9\84\D9\84\D8\AA\D8\B9\D8\AF\D9\8A\D9\84.load", 1
  br i1 %cmpeqtmp, label %then_6, label %else_8

then_6:                                           ; preds = %entry
  %len1 = call i64 @strlen(ptr @str.const.25)
  %len2 = call i64 @strlen(ptr %"\D9\85\D8\B3\D8\AA\D8\B9\D9\8A\D8\B12")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.25, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D9\85\D8\B3\D8\AA\D8\B9\D9\8A\D8\B12", i64 %copylen2)
  %len14 = call i64 @strlen(ptr %concat.buf)
  %len25 = call i64 @strlen(ptr @str.const.26)
  %totallen6 = add i64 %len14, %len25
  %bufsize7 = add i64 %totallen6, 1
  %concat.buf8 = alloca i8, i64 %bufsize7, align 1
  %2 = call ptr @memcpy(ptr %concat.buf8, ptr %concat.buf, i64 %len14)
  %dest29 = getelementptr i8, ptr %concat.buf8, i64 %len14
  %copylen210 = add i64 %len25, 1
  %3 = call ptr @memcpy(ptr %dest29, ptr @str.const.26, i64 %copylen210)
  %len111 = call i64 @strlen(ptr %concat.buf8)
  %len212 = call i64 @strlen(ptr %"\D9\85\D8\A7\D9\84\D9\831")
  %totallen13 = add i64 %len111, %len212
  %bufsize14 = add i64 %totallen13, 1
  %concat.buf15 = alloca i8, i64 %bufsize14, align 1
  %4 = call ptr @memcpy(ptr %concat.buf15, ptr %concat.buf8, i64 %len111)
  %dest216 = getelementptr i8, ptr %concat.buf15, i64 %len111
  %copylen217 = add i64 %len212, 1
  %5 = call ptr @memcpy(ptr %dest216, ptr %"\D9\85\D8\A7\D9\84\D9\831", i64 %copylen217)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf15)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_7

else_8:                                           ; preds = %entry
  %len118 = call i64 @strlen(ptr @str.const.30)
  %len219 = call i64 @strlen(ptr %"\D9\85\D8\B3\D8\AA\D8\B9\D9\8A\D8\B12")
  %totallen20 = add i64 %len118, %len219
  %bufsize21 = add i64 %totallen20, 1
  %concat.buf22 = alloca i8, i64 %bufsize21, align 1
  %8 = call ptr @memcpy(ptr %concat.buf22, ptr @str.const.30, i64 %len118)
  %dest223 = getelementptr i8, ptr %concat.buf22, i64 %len118
  %copylen224 = add i64 %len219, 1
  %9 = call ptr @memcpy(ptr %dest223, ptr %"\D9\85\D8\B3\D8\AA\D8\B9\D9\8A\D8\B12", i64 %copylen224)
  %len125 = call i64 @strlen(ptr %concat.buf22)
  %len226 = call i64 @strlen(ptr @str.const.26)
  %totallen27 = add i64 %len125, %len226
  %bufsize28 = add i64 %totallen27, 1
  %concat.buf29 = alloca i8, i64 %bufsize28, align 1
  %10 = call ptr @memcpy(ptr %concat.buf29, ptr %concat.buf22, i64 %len125)
  %dest230 = getelementptr i8, ptr %concat.buf29, i64 %len125
  %copylen231 = add i64 %len226, 1
  %11 = call ptr @memcpy(ptr %dest230, ptr @str.const.26, i64 %copylen231)
  %len132 = call i64 @strlen(ptr %concat.buf29)
  %len233 = call i64 @strlen(ptr %"\D9\85\D8\A7\D9\84\D9\831")
  %totallen34 = add i64 %len132, %len233
  %bufsize35 = add i64 %totallen34, 1
  %concat.buf36 = alloca i8, i64 %bufsize35, align 1
  %12 = call ptr @memcpy(ptr %concat.buf36, ptr %concat.buf29, i64 %len132)
  %dest237 = getelementptr i8, ptr %concat.buf36, i64 %len132
  %copylen238 = add i64 %len233, 1
  %13 = call ptr @memcpy(ptr %dest237, ptr %"\D9\85\D8\A7\D9\84\D9\831", i64 %copylen238)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf36)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_7

merge_7:                                          ; preds = %else_8, %then_6
  ret i64 0
}

define i64 @"\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1_\D9\85\D9\88\D8\B1\D8\AF"(ptr %"\D8\A7\D8\B3\D9\85_\D9\85\D9\88\D8\B1\D8\AF") {
entry:
  %"\D8\A7\D8\B3\D9\85_\D9\85\D9\88\D8\B1\D8\AF1" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85_\D9\85\D9\88\D8\B1\D8\AF", ptr %"\D8\A7\D8\B3\D9\85_\D9\85\D9\88\D8\B1\D8\AF1", align 8
  %len1 = call i64 @strlen(ptr @str.const.35)
  %len2 = call i64 @strlen(ptr %"\D8\A7\D8\B3\D9\85_\D9\85\D9\88\D8\B1\D8\AF1")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.35, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\A7\D8\B3\D9\85_\D9\85\D9\88\D8\B1\D8\AF1", i64 %copylen2)
  %len12 = call i64 @strlen(ptr %concat.buf)
  %len23 = call i64 @strlen(ptr @str.const.36)
  %totallen4 = add i64 %len12, %len23
  %bufsize5 = add i64 %totallen4, 1
  %concat.buf6 = alloca i8, i64 %bufsize5, align 1
  %2 = call ptr @memcpy(ptr %concat.buf6, ptr %concat.buf, i64 %len12)
  %dest27 = getelementptr i8, ptr %concat.buf6, i64 %len12
  %copylen28 = add i64 %len23, 1
  %3 = call ptr @memcpy(ptr %dest27, ptr @str.const.36, i64 %copylen28)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf6)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D9\86\D8\B7\D8\A7\D9\82\D8\A7\D8\AA_\D9\85\D8\AA\D8\AF\D8\A7\D8\AE\D9\84\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.40)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.44)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call ptr @"\D8\AA\D8\B3\D8\AC\D9\8A\D9\84_\D8\AF\D8\AE\D9\88\D9\84"(ptr @str.const.48, i64 0)
  %calltmp1 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\AA\D8\BA\D9\8A\D8\B1"(ptr @str.const.49, ptr @str.const.50, i64 1, i64 20)
  %calltmp2 = call ptr @"\D8\AA\D8\B3\D8\AC\D9\8A\D9\84_\D8\AF\D8\AE\D9\88\D9\84"(ptr @str.const.51, i64 1)
  %calltmp3 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\B1\D8\AC\D8\B9"(ptr @str.const.52, ptr @str.const.49, i64 5, i64 10)
  %calltmp4 = call i64 @"\D8\AA\D8\AD\D9\82\D9\82_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9"(ptr @str.const.49, ptr @str.const.52, i64 0)
  %calltmp5 = call ptr @"\D8\AA\D8\B3\D8\AC\D9\8A\D9\84_\D8\AE\D8\B1\D9\88\D8\AC"(ptr @str.const.51, i64 1)
  %calltmp6 = call i64 @"\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1_\D9\85\D9\88\D8\B1\D8\AF"(ptr @str.const.52)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.58)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp7 = call ptr @"\D8\AA\D8\B3\D8\AC\D9\8A\D9\84_\D8\AF\D8\AE\D9\88\D9\84"(ptr @str.const.62, i64 1)
  %calltmp8 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\B1\D8\AC\D8\B9"(ptr @str.const.63, ptr @str.const.49, i64 12, i64 18)
  %calltmp9 = call i64 @"\D8\AA\D8\AD\D9\82\D9\82_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9"(ptr @str.const.49, ptr @str.const.63, i64 1)
  %calltmp10 = call ptr @"\D8\AA\D8\B3\D8\AC\D9\8A\D9\84_\D8\AF\D8\AE\D9\88\D9\84"(ptr @str.const.67, i64 2)
  %calltmp11 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\AA\D8\BA\D9\8A\D8\B1"(ptr @str.const.68, ptr @str.const.69, i64 14, i64 16)
  %calltmp12 = call ptr @"\D8\AA\D8\B3\D8\AC\D9\8A\D9\84_\D8\AE\D8\B1\D9\88\D8\AC"(ptr @str.const.67, i64 2)
  %calltmp13 = call i64 @"\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1_\D9\85\D9\88\D8\B1\D8\AF"(ptr @str.const.68)
  %calltmp14 = call ptr @"\D8\AA\D8\B3\D8\AC\D9\8A\D9\84_\D8\AE\D8\B1\D9\88\D8\AC"(ptr @str.const.62, i64 1)
  %calltmp15 = call i64 @"\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1_\D9\85\D9\88\D8\B1\D8\AF"(ptr @str.const.63)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.74)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp16 = call ptr @"\D8\AA\D8\B3\D8\AC\D9\8A\D9\84_\D8\AE\D8\B1\D9\88\D8\AC"(ptr @str.const.48, i64 0)
  %calltmp17 = call i64 @"\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1_\D9\85\D9\88\D8\B1\D8\AF"(ptr @str.const.49)
  ret i64 0
}

define i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D9\85\D9\84\D9\83\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.80)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.84)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.88)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\AA\D8\BA\D9\8A\D8\B1"(ptr @str.const.92, ptr @str.const.93, i64 1, i64 5)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.94)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.98)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.102)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp1 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\AA\D8\BA\D9\8A\D8\B1"(ptr @str.const.106, ptr @str.const.93, i64 3, i64 10)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.108)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp2 = call i64 @"\D8\AA\D8\AD\D9\82\D9\82_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9"(ptr @str.const.106, ptr @str.const.113, i64 0)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.114)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.118)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp3 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\AA\D8\BA\D9\8A\D8\B1"(ptr @str.const.122, ptr @str.const.93, i64 6, i64 10)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.124)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.128)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\B9\D8\B1\D8\B6_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9"(ptr %"\D9\88\D8\B5\D9\81", i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\81\D8\A7\D8\A9") {
entry:
  %"\D9\85\D8\B3\D8\AA\D9\88\D9\81\D8\A7\D8\A92" = alloca i64, align 8
  store i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\81\D8\A7\D8\A9", ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\81\D8\A7\D8\A92", align 8
  %"\D9\88\D8\B5\D9\811" = alloca ptr, align 8
  store ptr %"\D9\88\D8\B5\D9\81", ptr %"\D9\88\D8\B5\D9\811", align 8
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\81\D8\A7\D8\A9.load" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\81\D8\A7\D8\A92", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\81\D8\A7\D8\A9.load", 1
  br i1 %cmpeqtmp, label %then_9, label %else_11

then_9:                                           ; preds = %entry
  %len1 = call i64 @strlen(ptr @str.const.132)
  %len2 = call i64 @strlen(ptr %"\D9\88\D8\B5\D9\811")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.132, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D9\88\D8\B5\D9\811", i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_10

else_11:                                          ; preds = %entry
  %len13 = call i64 @strlen(ptr @str.const.136)
  %len24 = call i64 @strlen(ptr %"\D9\88\D8\B5\D9\811")
  %totallen5 = add i64 %len13, %len24
  %bufsize6 = add i64 %totallen5, 1
  %concat.buf7 = alloca i8, i64 %bufsize6, align 1
  %4 = call ptr @memcpy(ptr %concat.buf7, ptr @str.const.136, i64 %len13)
  %dest28 = getelementptr i8, ptr %concat.buf7, i64 %len13
  %copylen29 = add i64 %len24, 1
  %5 = call ptr @memcpy(ptr %dest28, ptr %"\D9\88\D8\B5\D9\811", i64 %copylen29)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf7)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_10

merge_10:                                         ; preds = %else_11, %then_9
  ret i64 0
}

define i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D9\82\D9\88\D8\A7\D8\B9\D8\AF"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.140)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.144)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %calltmp = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9"(ptr @str.const.148, i64 1)
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load", 1
  store i64 %addtmp, ptr %alloca, align 8
  %calltmp1 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9"(ptr @str.const.149, i64 1)
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load2" = load i64, ptr %alloca, align 8
  %addtmp3 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load2", 1
  store i64 %addtmp3, ptr %alloca, align 8
  %calltmp4 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9"(ptr @str.const.150, i64 1)
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load5" = load i64, ptr %alloca, align 8
  %addtmp6 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load5", 1
  store i64 %addtmp6, ptr %alloca, align 8
  %calltmp7 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9"(ptr @str.const.151, i64 1)
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load8" = load i64, ptr %alloca, align 8
  %addtmp9 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load8", 1
  store i64 %addtmp9, ptr %alloca, align 8
  %calltmp10 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9"(ptr @str.const.152, i64 1)
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load11" = load i64, ptr %alloca, align 8
  %addtmp12 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load11", 1
  store i64 %addtmp12, ptr %alloca, align 8
  %calltmp13 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9"(ptr @str.const.153, i64 1)
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load14" = load i64, ptr %alloca, align 8
  %addtmp15 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load14", 1
  store i64 %addtmp15, ptr %alloca, align 8
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.154)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.162)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.166)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.170)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.174)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.178)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D9\86\D8\B7\D8\A7\D9\82\D8\A7\D8\AA_\D9\85\D8\AA\D8\AF\D8\A7\D8\AE\D9\84\D8\A9"()
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.182)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp1 = call i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D9\85\D9\84\D9\83\D9\8A\D8\A9"()
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.186)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp2 = call i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D9\82\D9\88\D8\A7\D8\B9\D8\AF"()
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.190)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
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
