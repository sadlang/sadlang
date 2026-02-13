; ModuleID = '05_complex_task_scheduler'
source_filename = "05_complex_task_scheduler"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [9 x i8] c"  [\E2\9C\93] \00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.1 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.3 = private unnamed_addr constant [9 x i8] c"  [\E2\9C\97] \00", align 1
@str.const.7 = private unnamed_addr constant [9 x i8] c"    \E2\94\82 \00", align 1
@str.const.8 = private unnamed_addr constant [16 x i8] c" [\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9:\00", align 1
@str.const.9 = private unnamed_addr constant [5 x i8] c"] - \00", align 1
@str.ret = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.12 = private unnamed_addr constant [39 x i8] c"=== \D8\AA\D9\86\D9\81\D9\8A\D8\B0 \D9\85\D9\87\D8\A7\D9\85 \D8\A8\D8\B3\D9\8A\D8\B7\D8\A9 ===\00", align 1
@str.const.16 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.20 = private unnamed_addr constant [40 x i8] c"  \D9\85\D9\87\D9\85\D8\A9 \D8\AD\D8\B3\D8\A7\D8\A8 (10 \D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA): \00", align 1
@str.const.24 = private unnamed_addr constant [18 x i8] c"\D9\85\D9\87\D9\85\D8\A9 \D8\AD\D8\B3\D8\A7\D8\A8\00", align 1
@str.const.26 = private unnamed_addr constant [45 x i8] c"  \D9\85\D9\87\D9\85\D8\A9 \D8\A8\D8\AD\D8\AB \D8\B9\D9\86 15 \D9\81\D9\8A 20 \D8\B9\D9\86\D8\B5\D8\B1: \00", align 1
@str.const.30 = private unnamed_addr constant [25 x i8] c"\D9\85\D9\87\D9\85\D8\A9 \D8\A8\D8\AD\D8\AB \D9\86\D8\A7\D8\AC\D8\AD\00", align 1
@str.const.32 = private unnamed_addr constant [25 x i8] c"\D9\85\D9\87\D9\85\D8\A9 \D8\A8\D8\AD\D8\AB \D9\81\D8\A7\D8\B4\D9\84\00", align 1
@str.const.34 = private unnamed_addr constant [50 x i8] c"  \D9\85\D9\87\D9\85\D8\A9 \D9\81\D8\B1\D8\B2 (5 \D8\B9\D9\86\D8\A7\D8\B5\D8\B1) \D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A7\D8\AA: \00", align 1
@str.const.38 = private unnamed_addr constant [16 x i8] c"\D9\85\D9\87\D9\85\D8\A9 \D9\81\D8\B1\D8\B2\00", align 1
@str.const.40 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.44 = private unnamed_addr constant [49 x i8] c"=== \D8\AC\D8\AF\D9\88\D9\84\D8\A9 \D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85 \D8\A8\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A7\D8\AA ===\00", align 1
@str.const.48 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.52 = private unnamed_addr constant [104 x i8] c"    \E2\94\8C\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\00", align 1
@str.const.56 = private unnamed_addr constant [18 x i8] c"\D8\AD\D8\B3\D8\A7\D8\A8_\D8\B9\D8\A7\D8\AC\D9\84\00", align 1
@str.const.57 = private unnamed_addr constant [11 x i8] c"\D9\85\D9\83\D8\AA\D9\85\D9\84\00", align 1
@str.const.58 = private unnamed_addr constant [16 x i8] c"\D8\A8\D8\AD\D8\AB_\D8\B9\D8\A7\D8\AF\D9\8A\00", align 1
@str.const.59 = private unnamed_addr constant [11 x i8] c"\D9\85\D9\86\D8\AA\D8\B8\D8\B1\00", align 1
@str.const.60 = private unnamed_addr constant [18 x i8] c"\D9\81\D8\B1\D8\B2_\D9\85\D9\86\D8\AE\D9\81\D8\B6\00", align 1
@str.const.62 = private unnamed_addr constant [22 x i8] c"\D9\86\D8\B3\D8\AE_\D8\A7\D8\AD\D8\AA\D9\8A\D8\A7\D8\B7\D9\8A\00", align 1
@str.const.63 = private unnamed_addr constant [11 x i8] c"\D9\85\D8\AC\D8\AF\D9\88\D9\84\00", align 1
@str.const.64 = private unnamed_addr constant [20 x i8] c"\D8\AA\D8\AD\D8\AF\D9\8A\D8\AB_\D9\81\D9\87\D8\B1\D8\B3\00", align 1
@str.const.65 = private unnamed_addr constant [22 x i8] c"\D9\82\D9\8A\D8\AF_\D8\A7\D9\84\D8\AA\D9\86\D9\81\D9\8A\D8\B0\00", align 1
@str.const.66 = private unnamed_addr constant [104 x i8] c"    \E2\94\94\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\00", align 1
@str.const.70 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.74 = private unnamed_addr constant [29 x i8] c"  \D9\86\D8\AA\D8\A7\D8\A6\D8\AC \D8\A7\D9\84\D8\AA\D9\86\D9\81\D9\8A\D8\B0:\00", align 1
@str.const.78 = private unnamed_addr constant [21 x i8] c"    \D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9 3: \00", align 1
@str.const.82 = private unnamed_addr constant [21 x i8] c"    \D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9 2: \00", align 1
@str.const.86 = private unnamed_addr constant [21 x i8] c"    \D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9 1: \00", align 1
@str.const.90 = private unnamed_addr constant [40 x i8] c"\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9 \D8\A3\D8\B9\D9\84\D9\89 = \D8\B9\D9\85\D9\84 \D8\A3\D9\83\D8\AB\D8\B1\00", align 1
@str.const.92 = private unnamed_addr constant [39 x i8] c"\D8\AA\D8\B1\D8\AA\D9\8A\D8\A8 \D8\A7\D9\84\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A7\D8\AA \D8\B5\D8\AD\D9\8A\D8\AD\00", align 1
@str.const.94 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.98 = private unnamed_addr constant [45 x i8] c"=== \D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9 \D8\AA\D9\86\D8\A7\D9\81\D8\B3 \D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85 ===\00", align 1
@str.const.102 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.106 = private unnamed_addr constant [19 x i8] c"  \D8\A7\D9\84\D9\85\D9\87\D9\85\D8\A9 1: \00", align 1
@str.const.107 = private unnamed_addr constant [2 x i8] c"/\00", align 1
@str.const.111 = private unnamed_addr constant [19 x i8] c"  \D8\A7\D9\84\D9\85\D9\87\D9\85\D8\A9 2: \00", align 1
@str.const.116 = private unnamed_addr constant [19 x i8] c"  \D8\A7\D9\84\D9\85\D9\87\D9\85\D8\A9 3: \00", align 1
@str.const.121 = private unnamed_addr constant [28 x i8] c"\D8\A7\D9\84\D9\85\D9\87\D9\85\D8\A9 1 \D8\A7\D9\83\D8\AA\D9\85\D9\84\D8\AA\00", align 1
@str.const.123 = private unnamed_addr constant [28 x i8] c"\D8\A7\D9\84\D9\85\D9\87\D9\85\D8\A9 2 \D8\A7\D9\83\D8\AA\D9\85\D9\84\D8\AA\00", align 1
@str.const.125 = private unnamed_addr constant [44 x i8] c"\D8\A7\D9\84\D9\85\D9\87\D9\85\D8\A9 3 \D9\84\D9\85 \D8\AA\D9\83\D8\AA\D9\85\D9\84 (\D9\85\D8\AA\D9\88\D9\82\D8\B9)\00", align 1
@str.const.127 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.131 = private unnamed_addr constant [40 x i8] c"=== \D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA \D8\A7\D9\84\D8\AC\D8\AF\D9\88\D9\84\D8\A9 ===\00", align 1
@str.const.135 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.139 = private unnamed_addr constant [108 x i8] c"  \E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.143 = private unnamed_addr constant [60 x i8] c"  \E2\95\91  \D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85 \D8\A7\D9\84\D9\85\D8\AA\D8\B2\D8\A7\D9\85\D9\86\D8\A9       \E2\95\91\00", align 1
@str.const.147 = private unnamed_addr constant [108 x i8] c"  \E2\95\A0\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\A3\00", align 1
@str.const.151 = private unnamed_addr constant [22 x i8] c"  \E2\95\91  \D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9: \00", align 1
@str.const.152 = private unnamed_addr constant [22 x i8] c"                  \E2\95\91\00", align 1
@str.const.156 = private unnamed_addr constant [22 x i8] c"  \E2\95\91  \D9\85\D9\86\D8\AA\D8\B8\D8\B1\D8\A9: \00", align 1
@str.const.161 = private unnamed_addr constant [20 x i8] c"  \E2\95\91  \D9\85\D9\84\D8\BA\D8\A7\D8\A9: \00", align 1
@str.const.162 = private unnamed_addr constant [23 x i8] c"                   \E2\95\91\00", align 1
@str.const.166 = private unnamed_addr constant [22 x i8] c"  \E2\95\91  \D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A: \00", align 1
@str.const.167 = private unnamed_addr constant [21 x i8] c"                 \E2\95\91\00", align 1
@str.const.171 = private unnamed_addr constant [33 x i8] c"  \E2\95\91  \D9\86\D8\B3\D8\A8\D8\A9 \D8\A7\D9\84\D8\A5\D9\86\D8\AC\D8\A7\D8\B2: \00", align 1
@str.const.172 = private unnamed_addr constant [14 x i8] c"%         \E2\95\91\00", align 1
@str.const.176 = private unnamed_addr constant [108 x i8] c"  \E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.180 = private unnamed_addr constant [35 x i8] c"\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A \D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85 \D8\B5\D8\AD\D9\8A\D8\AD\00", align 1
@str.const.182 = private unnamed_addr constant [33 x i8] c"\D9\86\D8\B3\D8\A8\D8\A9 \D8\A7\D9\84\D8\A5\D9\86\D8\AC\D8\A7\D8\B2 \D8\AC\D9\8A\D8\AF\D8\A9\00", align 1
@str.const.184 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.188 = private unnamed_addr constant [133 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.192 = private unnamed_addr constant [79 x i8] c"\E2\95\91  \D9\86\D8\B8\D8\A7\D9\85 \D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9 \D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85 \D8\A7\D9\84\D9\85\D8\AA\D8\B2\D8\A7\D9\85\D9\86\D8\A9 \D8\A7\D9\84\D9\85\D8\B9\D9\82\D8\AF    \E2\95\91\00", align 1
@str.const.196 = private unnamed_addr constant [133 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.200 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.204 = private unnamed_addr constant [56 x i8] c"=== \D8\A7\D9\86\D8\AA\D9\87\D8\AA \D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9 \D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85 \D8\A8\D9\86\D8\AC\D8\A7\D8\AD ===\00", align 1

define i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr %"\D9\88\D8\B5\D9\81", i64 %"\D9\86\D8\A7\D8\AC\D8\AD") {
entry:
  %"\D9\86\D8\A7\D8\AC\D8\AD2" = alloca i64, align 8
  store i64 %"\D9\86\D8\A7\D8\AC\D8\AD", ptr %"\D9\86\D8\A7\D8\AC\D8\AD2", align 8
  %"\D9\88\D8\B5\D9\811" = alloca ptr, align 8
  store ptr %"\D9\88\D8\B5\D9\81", ptr %"\D9\88\D8\B5\D9\811", align 8
  %"%\D9\86\D8\A7\D8\AC\D8\AD.load" = load i64, ptr %"\D9\86\D8\A7\D8\AC\D8\AD2", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86\D8\A7\D8\AC\D8\AD.load", 1
  br i1 %cmpeqtmp, label %then_0, label %else_2

then_0:                                           ; preds = %entry
  %len1 = call i64 @strlen(ptr @str.const)
  %len2 = call i64 @strlen(ptr %"\D9\88\D8\B5\D9\811")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D9\88\D8\B5\D9\811", i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_1

else_2:                                           ; preds = %entry
  %len13 = call i64 @strlen(ptr @str.const.3)
  %len24 = call i64 @strlen(ptr %"\D9\88\D8\B5\D9\811")
  %totallen5 = add i64 %len13, %len24
  %bufsize6 = add i64 %totallen5, 1
  %concat.buf7 = alloca i8, i64 %bufsize6, align 1
  %4 = call ptr @memcpy(ptr %concat.buf7, ptr @str.const.3, i64 %len13)
  %dest28 = getelementptr i8, ptr %concat.buf7, i64 %len13
  %copylen29 = add i64 %len24, 1
  %5 = call ptr @memcpy(ptr %dest28, ptr %"\D9\88\D8\B5\D9\811", i64 %copylen29)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf7)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_1

merge_1:                                          ; preds = %else_2, %then_0
  ret i64 0
}

define i64 @"\D9\86\D9\81\D8\B0_\D9\85\D9\87\D9\85\D8\A9_\D8\AD\D8\B3\D8\A7\D8\A8"(i64 %"\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA") {
entry:
  %"\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA1" = alloca i64, align 8
  store i64 %"\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA", ptr %"\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA1", align 8
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %alloca2 = alloca i64, align 8
  store i64 0, ptr %alloca2, align 8
  br label %while_cond_3

while_cond_3:                                     ; preds = %while_body_4, %entry
  %"%\D9\85.load" = load i64, ptr %alloca2, align 8
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load" = load i64, ptr %"\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA1", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85.load", %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load"
  br i1 %cmplttmp, label %while_body_4, label %while_exit_5

while_body_4:                                     ; preds = %while_cond_3
  %"%\D9\85.load3" = load i64, ptr %alloca2, align 8
  %multmp = mul i64 %"%\D9\85.load3", 2
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", %multmp
  store i64 %addtmp, ptr %alloca, align 8
  %"%\D9\85.load4" = load i64, ptr %alloca2, align 8
  %addtmp5 = add i64 %"%\D9\85.load4", 1
  store i64 %addtmp5, ptr %alloca2, align 8
  br label %while_cond_3

while_exit_5:                                     ; preds = %while_cond_3
  %loadtmp = load i64, ptr %alloca, align 8
  ret i64 %loadtmp
}

define i64 @"\D9\86\D9\81\D8\B0_\D9\85\D9\87\D9\85\D8\A9_\D8\A8\D8\AD\D8\AB"(i64 %"\D8\B9\D8\AF\D8\AF_\D8\B9\D9\86\D8\A7\D8\B5\D8\B1", i64 %"\D9\87\D8\AF\D9\81") {
entry:
  %"\D9\87\D8\AF\D9\812" = alloca i64, align 8
  store i64 %"\D9\87\D8\AF\D9\81", ptr %"\D9\87\D8\AF\D9\812", align 8
  %"\D8\B9\D8\AF\D8\AF_\D8\B9\D9\86\D8\A7\D8\B5\D8\B11" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF_\D8\B9\D9\86\D8\A7\D8\B5\D8\B1", ptr %"\D8\B9\D8\AF\D8\AF_\D8\B9\D9\86\D8\A7\D8\B5\D8\B11", align 8
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  br label %while_cond_6

while_cond_6:                                     ; preds = %merge_10, %entry
  %"%\D9\85.load" = load i64, ptr %alloca, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\B9\D9\86\D8\A7\D8\B5\D8\B1.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\B9\D9\86\D8\A7\D8\B5\D8\B11", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85.load", %"%\D8\B9\D8\AF\D8\AF_\D8\B9\D9\86\D8\A7\D8\B5\D8\B1.load"
  br i1 %cmplttmp, label %while_body_7, label %while_exit_8

while_body_7:                                     ; preds = %while_cond_6
  %"%\D9\85.load3" = load i64, ptr %alloca, align 8
  %"%\D9\87\D8\AF\D9\81.load" = load i64, ptr %"\D9\87\D8\AF\D9\812", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\85.load3", %"%\D9\87\D8\AF\D9\81.load"
  br i1 %cmpeqtmp, label %then_9, label %merge_10

while_exit_8:                                     ; preds = %while_cond_6
  ret i64 -1

then_9:                                           ; preds = %while_body_7
  %loadtmp = load i64, ptr %alloca, align 8
  ret i64 %loadtmp

merge_10:                                         ; preds = %while_body_7
  %"%\D9\85.load4" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D9\85.load4", 1
  store i64 %addtmp, ptr %alloca, align 8
  br label %while_cond_6
}

define i64 @"\D9\86\D9\81\D8\B0_\D9\85\D9\87\D9\85\D8\A9_\D9\81\D8\B1\D8\B2"(i64 %"\D8\B9\D8\AF\D8\AF_\D8\B9\D9\86\D8\A7\D8\B5\D8\B1") {
entry:
  %"\D8\B9\D8\AF\D8\AF_\D8\B9\D9\86\D8\A7\D8\B5\D8\B11" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF_\D8\B9\D9\86\D8\A7\D8\B5\D8\B1", ptr %"\D8\B9\D8\AF\D8\AF_\D8\B9\D9\86\D8\A7\D8\B5\D8\B11", align 8
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %alloca2 = alloca i64, align 8
  store i64 0, ptr %alloca2, align 8
  br label %while_cond_11

while_cond_11:                                    ; preds = %while_exit_16, %entry
  %"%\D9\85.load" = load i64, ptr %alloca2, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\B9\D9\86\D8\A7\D8\B5\D8\B1.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\B9\D9\86\D8\A7\D8\B5\D8\B11", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85.load", %"%\D8\B9\D8\AF\D8\AF_\D8\B9\D9\86\D8\A7\D8\B5\D8\B1.load"
  br i1 %cmplttmp, label %while_body_12, label %while_exit_13

while_body_12:                                    ; preds = %while_cond_11
  %alloca3 = alloca i64, align 8
  store i64 0, ptr %alloca3, align 8
  br label %while_cond_14

while_exit_13:                                    ; preds = %while_cond_11
  %loadtmp = load i64, ptr %alloca, align 8
  ret i64 %loadtmp

while_cond_14:                                    ; preds = %while_body_15, %while_body_12
  %"%\D8\B9\D8\AF\D8\AF_\D8\B9\D9\86\D8\A7\D8\B5\D8\B1.load4" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\B9\D9\86\D8\A7\D8\B5\D8\B11", align 8
  %"%\D9\85.load5" = load i64, ptr %alloca2, align 8
  %subtmp = sub i64 %"%\D8\B9\D8\AF\D8\AF_\D8\B9\D9\86\D8\A7\D8\B5\D8\B1.load4", %"%\D9\85.load5"
  %subtmp6 = sub i64 %subtmp, 1
  %"%\D8\AC.load" = load i64, ptr %alloca3, align 8
  %cmplttmp7 = icmp slt i64 %"%\D8\AC.load", %subtmp6
  br i1 %cmplttmp7, label %while_body_15, label %while_exit_16

while_body_15:                                    ; preds = %while_cond_14
  %"%\D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A7\D8\AA.load" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A7\D8\AA.load", 1
  store i64 %addtmp, ptr %alloca, align 8
  %"%\D8\AC.load8" = load i64, ptr %alloca3, align 8
  %addtmp9 = add i64 %"%\D8\AC.load8", 1
  store i64 %addtmp9, ptr %alloca3, align 8
  br label %while_cond_14

while_exit_16:                                    ; preds = %while_cond_14
  %"%\D9\85.load10" = load i64, ptr %alloca2, align 8
  %addtmp11 = add i64 %"%\D9\85.load10", 1
  store i64 %addtmp11, ptr %alloca2, align 8
  br label %while_cond_11
}

define ptr @"\D8\B7\D8\A8\D8\A7\D8\B9\D8\A9_\D9\85\D9\87\D9\85\D8\A9"(ptr %"\D8\A7\D8\B3\D9\85_\D9\85", i64 %"\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9", ptr %"\D8\AD\D8\A7\D9\84\D8\A9_\D9\85") {
entry:
  %"\D8\AD\D8\A7\D9\84\D8\A9_\D9\853" = alloca ptr, align 8
  store ptr %"\D8\AD\D8\A7\D9\84\D8\A9_\D9\85", ptr %"\D8\AD\D8\A7\D9\84\D8\A9_\D9\853", align 8
  %"\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A92" = alloca i64, align 8
  store i64 %"\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9", ptr %"\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A92", align 8
  %"\D8\A7\D8\B3\D9\85_\D9\851" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85_\D9\85", ptr %"\D8\A7\D8\B3\D9\85_\D9\851", align 8
  %len1 = call i64 @strlen(ptr @str.const.7)
  %len2 = call i64 @strlen(ptr %"\D8\A7\D8\B3\D9\85_\D9\851")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.7, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\A7\D8\B3\D9\85_\D9\851", i64 %copylen2)
  %len14 = call i64 @strlen(ptr %concat.buf)
  %len25 = call i64 @strlen(ptr @str.const.8)
  %totallen6 = add i64 %len14, %len25
  %bufsize7 = add i64 %totallen6, 1
  %concat.buf8 = alloca i8, i64 %bufsize7, align 1
  %2 = call ptr @memcpy(ptr %concat.buf8, ptr %concat.buf, i64 %len14)
  %dest29 = getelementptr i8, ptr %concat.buf8, i64 %len14
  %copylen210 = add i64 %len25, 1
  %3 = call ptr @memcpy(ptr %dest29, ptr @str.const.8, i64 %copylen210)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret ptr @str.ret
}

define i64 @"\D8\AA\D9\86\D9\81\D9\8A\D8\B0_\D8\A8\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9"(i64 %"\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9", i64 %"\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA") {
entry:
  %"\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA2" = alloca i64, align 8
  store i64 %"\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA", ptr %"\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA2", align 8
  %"\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A91" = alloca i64, align 8
  store i64 %"\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9", ptr %"\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A91", align 8
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load" = load i64, ptr %"\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA2", align 8
  %"%\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9.load" = load i64, ptr %"\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A91", align 8
  %multmp = mul i64 %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load", %"%\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9.load"
  %alloca = alloca i64, align 8
  store i64 %multmp, ptr %alloca, align 8
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D9\81\D8\B9\D9\84\D9\8A\D8\A9.load" = load i64, ptr %alloca, align 8
  %calltmp = call i64 @"\D9\86\D9\81\D8\B0_\D9\85\D9\87\D9\85\D8\A9_\D8\AD\D8\B3\D8\A7\D8\A8"(i64 %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D9\81\D8\B9\D9\84\D9\8A\D8\A9.load")
  ret i64 %calltmp
}

define i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D9\85\D9\87\D8\A7\D9\85_\D8\A8\D8\B3\D9\8A\D8\B7\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.16)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call i64 @"\D9\86\D9\81\D8\B0_\D9\85\D9\87\D9\85\D8\A9_\D8\AD\D8\B3\D8\A7\D8\A8"(i64 10)
  %alloca = alloca i64, align 8
  store i64 %calltmp, ptr %alloca, align 8
  %alloca1 = alloca ptr, align 8
  %len1 = call i64 @strlen(ptr @str.const.20)
  %len2 = call i64 @strlen(ptr %alloca1)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %4 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.20, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %5 = call ptr @memcpy(ptr %dest2, ptr %alloca1, i64 %copylen2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\861.load" = load i64, ptr %alloca, align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\861.load", 0
  br i1 %cmpgttmp, label %then_17, label %else_19

then_17:                                          ; preds = %entry
  %calltmp2 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.24, i64 1)
  br label %merge_18

else_19:                                          ; preds = %entry
  %calltmp3 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.24, i64 0)
  br label %merge_18

merge_18:                                         ; preds = %else_19, %then_17
  %calltmp4 = call i64 @"\D9\86\D9\81\D8\B0_\D9\85\D9\87\D9\85\D8\A9_\D8\A8\D8\AD\D8\AB"(i64 20, i64 15)
  %alloca5 = alloca i64, align 8
  store i64 %calltmp4, ptr %alloca5, align 8
  %alloca6 = alloca ptr, align 8
  %len17 = call i64 @strlen(ptr @str.const.26)
  %len28 = call i64 @strlen(ptr %alloca6)
  %totallen9 = add i64 %len17, %len28
  %bufsize10 = add i64 %totallen9, 1
  %concat.buf11 = alloca i8, i64 %bufsize10, align 1
  %8 = call ptr @memcpy(ptr %concat.buf11, ptr @str.const.26, i64 %len17)
  %dest212 = getelementptr i8, ptr %concat.buf11, i64 %len17
  %copylen213 = add i64 %len28, 1
  %9 = call ptr @memcpy(ptr %dest212, ptr %alloca6, i64 %copylen213)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf11)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\862.load" = load i64, ptr %alloca5, align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\862.load", 15
  br i1 %cmpeqtmp, label %then_20, label %else_22

then_20:                                          ; preds = %merge_18
  %calltmp14 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.30, i64 1)
  br label %merge_21

else_22:                                          ; preds = %merge_18
  %calltmp15 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.30, i64 0)
  br label %merge_21

merge_21:                                         ; preds = %else_22, %then_20
  %calltmp16 = call i64 @"\D9\86\D9\81\D8\B0_\D9\85\D9\87\D9\85\D8\A9_\D8\A8\D8\AD\D8\AB"(i64 10, i64 20)
  %alloca17 = alloca i64, align 8
  store i64 %calltmp16, ptr %alloca17, align 8
  %"%\D9\863.load" = load i64, ptr %alloca17, align 8
  %cmplttmp = icmp slt i64 %"%\D9\863.load", 0
  br i1 %cmplttmp, label %then_23, label %else_25

then_23:                                          ; preds = %merge_21
  %calltmp18 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.32, i64 1)
  br label %merge_24

else_25:                                          ; preds = %merge_21
  %calltmp19 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.32, i64 0)
  br label %merge_24

merge_24:                                         ; preds = %else_25, %then_23
  %calltmp20 = call i64 @"\D9\86\D9\81\D8\B0_\D9\85\D9\87\D9\85\D8\A9_\D9\81\D8\B1\D8\B2"(i64 5)
  %alloca21 = alloca i64, align 8
  store i64 %calltmp20, ptr %alloca21, align 8
  %alloca22 = alloca ptr, align 8
  %len123 = call i64 @strlen(ptr @str.const.34)
  %len224 = call i64 @strlen(ptr %alloca22)
  %totallen25 = add i64 %len123, %len224
  %bufsize26 = add i64 %totallen25, 1
  %concat.buf27 = alloca i8, i64 %bufsize26, align 1
  %12 = call ptr @memcpy(ptr %concat.buf27, ptr @str.const.34, i64 %len123)
  %dest228 = getelementptr i8, ptr %concat.buf27, i64 %len123
  %copylen229 = add i64 %len224, 1
  %13 = call ptr @memcpy(ptr %dest228, ptr %alloca22, i64 %copylen229)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf27)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\864.load" = load i64, ptr %alloca21, align 8
  %cmpeqtmp30 = icmp eq i64 %"%\D9\864.load", 10
  br i1 %cmpeqtmp30, label %then_26, label %else_28

then_26:                                          ; preds = %merge_24
  %calltmp31 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.38, i64 1)
  br label %merge_27

else_28:                                          ; preds = %merge_24
  %calltmp32 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.38, i64 0)
  br label %merge_27

merge_27:                                         ; preds = %else_28, %then_26
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.40)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D8\AC\D8\AF\D9\88\D9\84\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.44)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.48)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.52)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call ptr @"\D8\B7\D8\A8\D8\A7\D8\B9\D8\A9_\D9\85\D9\87\D9\85\D8\A9"(ptr @str.const.56, i64 3, ptr @str.const.57)
  %calltmp1 = call ptr @"\D8\B7\D8\A8\D8\A7\D8\B9\D8\A9_\D9\85\D9\87\D9\85\D8\A9"(ptr @str.const.58, i64 2, ptr @str.const.59)
  %calltmp2 = call ptr @"\D8\B7\D8\A8\D8\A7\D8\B9\D8\A9_\D9\85\D9\87\D9\85\D8\A9"(ptr @str.const.60, i64 1, ptr @str.const.59)
  %calltmp3 = call ptr @"\D8\B7\D8\A8\D8\A7\D8\B9\D8\A9_\D9\85\D9\87\D9\85\D8\A9"(ptr @str.const.62, i64 1, ptr @str.const.63)
  %calltmp4 = call ptr @"\D8\B7\D8\A8\D8\A7\D8\B9\D8\A9_\D9\85\D9\87\D9\85\D8\A9"(ptr @str.const.64, i64 2, ptr @str.const.65)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.66)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.70)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp5 = call i64 @"\D8\AA\D9\86\D9\81\D9\8A\D8\B0_\D8\A8\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9"(i64 3, i64 5)
  %alloca = alloca i64, align 8
  store i64 %calltmp5, ptr %alloca, align 8
  %calltmp6 = call i64 @"\D8\AA\D9\86\D9\81\D9\8A\D8\B0_\D8\A8\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9"(i64 2, i64 5)
  %alloca7 = alloca i64, align 8
  store i64 %calltmp6, ptr %alloca7, align 8
  %calltmp8 = call i64 @"\D8\AA\D9\86\D9\81\D9\8A\D8\B0_\D8\A8\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9"(i64 1, i64 5)
  %alloca9 = alloca i64, align 8
  store i64 %calltmp8, ptr %alloca9, align 8
  %alloca10 = alloca ptr, align 8
  %alloca11 = alloca ptr, align 8
  %alloca12 = alloca ptr, align 8
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.74)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len1 = call i64 @strlen(ptr @str.const.78)
  %len2 = call i64 @strlen(ptr %alloca10)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %12 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.78, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %13 = call ptr @memcpy(ptr %dest2, ptr %alloca10, i64 %copylen2)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len113 = call i64 @strlen(ptr @str.const.82)
  %len214 = call i64 @strlen(ptr %alloca11)
  %totallen15 = add i64 %len113, %len214
  %bufsize16 = add i64 %totallen15, 1
  %concat.buf17 = alloca i8, i64 %bufsize16, align 1
  %16 = call ptr @memcpy(ptr %concat.buf17, ptr @str.const.82, i64 %len113)
  %dest218 = getelementptr i8, ptr %concat.buf17, i64 %len113
  %copylen219 = add i64 %len214, 1
  %17 = call ptr @memcpy(ptr %dest218, ptr %alloca11, i64 %copylen219)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf17)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len120 = call i64 @strlen(ptr @str.const.86)
  %len221 = call i64 @strlen(ptr %alloca12)
  %totallen22 = add i64 %len120, %len221
  %bufsize23 = add i64 %totallen22, 1
  %concat.buf24 = alloca i8, i64 %bufsize23, align 1
  %20 = call ptr @memcpy(ptr %concat.buf24, ptr @str.const.86, i64 %len120)
  %dest225 = getelementptr i8, ptr %concat.buf24, i64 %len120
  %copylen226 = add i64 %len221, 1
  %21 = call ptr @memcpy(ptr %dest225, ptr %alloca12, i64 %copylen226)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf24)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\861.load" = load i64, ptr %alloca, align 8
  %"%\D9\862.load" = load i64, ptr %alloca7, align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\861.load", %"%\D9\862.load"
  br i1 %cmpgttmp, label %then_29, label %else_31

then_29:                                          ; preds = %entry
  %calltmp27 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.90, i64 1)
  br label %merge_30

else_31:                                          ; preds = %entry
  %calltmp28 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.90, i64 0)
  br label %merge_30

merge_30:                                         ; preds = %else_31, %then_29
  %"%\D9\862.load29" = load i64, ptr %alloca7, align 8
  %"%\D9\863.load" = load i64, ptr %alloca9, align 8
  %cmpgttmp30 = icmp sgt i64 %"%\D9\862.load29", %"%\D9\863.load"
  br i1 %cmpgttmp30, label %then_32, label %else_34

then_32:                                          ; preds = %merge_30
  %calltmp31 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.92, i64 1)
  br label %merge_33

else_34:                                          ; preds = %merge_30
  %calltmp32 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.92, i64 0)
  br label %merge_33

merge_33:                                         ; preds = %else_34, %then_32
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.94)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D8\AA\D9\86\D8\A7\D9\81\D8\B3"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.98)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.102)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 0, ptr %alloca1, align 8
  %alloca2 = alloca i64, align 8
  store i64 0, ptr %alloca2, align 8
  %alloca3 = alloca i64, align 8
  store i64 0, ptr %alloca3, align 8
  %alloca4 = alloca i64, align 8
  store i64 15, ptr %alloca4, align 8
  %alloca5 = alloca i64, align 8
  store i64 10, ptr %alloca5, align 8
  %alloca6 = alloca i64, align 8
  store i64 20, ptr %alloca6, align 8
  br label %while_cond_35

while_cond_35:                                    ; preds = %while_body_36, %entry
  %"%\D8\AF\D9\88\D8\B1\D8\A9.load" = load i64, ptr %alloca, align 8
  %cmplttmp = icmp slt i64 %"%\D8\AF\D9\88\D8\B1\D8\A9.load", 10
  br i1 %cmplttmp, label %while_body_36, label %while_exit_37

while_body_36:                                    ; preds = %while_cond_35
  %"%\D9\85\D9\87\D9\85\D8\A91_\D8\AA\D9\82\D8\AF\D9\85.load" = load i64, ptr %alloca1, align 8
  %addtmp = add i64 %"%\D9\85\D9\87\D9\85\D8\A91_\D8\AA\D9\82\D8\AF\D9\85.load", 2
  store i64 %addtmp, ptr %alloca1, align 8
  %"%\D9\85\D9\87\D9\85\D8\A92_\D8\AA\D9\82\D8\AF\D9\85.load" = load i64, ptr %alloca2, align 8
  %addtmp7 = add i64 %"%\D9\85\D9\87\D9\85\D8\A92_\D8\AA\D9\82\D8\AF\D9\85.load", 3
  store i64 %addtmp7, ptr %alloca2, align 8
  %"%\D9\85\D9\87\D9\85\D8\A93_\D8\AA\D9\82\D8\AF\D9\85.load" = load i64, ptr %alloca3, align 8
  %addtmp8 = add i64 %"%\D9\85\D9\87\D9\85\D8\A93_\D8\AA\D9\82\D8\AF\D9\85.load", 1
  store i64 %addtmp8, ptr %alloca3, align 8
  %"%\D8\AF\D9\88\D8\B1\D8\A9.load9" = load i64, ptr %alloca, align 8
  %addtmp10 = add i64 %"%\D8\AF\D9\88\D8\B1\D8\A9.load9", 1
  store i64 %addtmp10, ptr %alloca, align 8
  br label %while_cond_35

while_exit_37:                                    ; preds = %while_cond_35
  %alloca11 = alloca ptr, align 8
  %alloca12 = alloca ptr, align 8
  %alloca13 = alloca ptr, align 8
  %alloca14 = alloca ptr, align 8
  %alloca15 = alloca ptr, align 8
  %alloca16 = alloca ptr, align 8
  %len1 = call i64 @strlen(ptr @str.const.106)
  %len2 = call i64 @strlen(ptr %alloca11)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %4 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.106, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %5 = call ptr @memcpy(ptr %dest2, ptr %alloca11, i64 %copylen2)
  %len117 = call i64 @strlen(ptr %concat.buf)
  %len218 = call i64 @strlen(ptr @str.const.107)
  %totallen19 = add i64 %len117, %len218
  %bufsize20 = add i64 %totallen19, 1
  %concat.buf21 = alloca i8, i64 %bufsize20, align 1
  %6 = call ptr @memcpy(ptr %concat.buf21, ptr %concat.buf, i64 %len117)
  %dest222 = getelementptr i8, ptr %concat.buf21, i64 %len117
  %copylen223 = add i64 %len218, 1
  %7 = call ptr @memcpy(ptr %dest222, ptr @str.const.107, i64 %copylen223)
  %len124 = call i64 @strlen(ptr %concat.buf21)
  %len225 = call i64 @strlen(ptr %alloca14)
  %totallen26 = add i64 %len124, %len225
  %bufsize27 = add i64 %totallen26, 1
  %concat.buf28 = alloca i8, i64 %bufsize27, align 1
  %8 = call ptr @memcpy(ptr %concat.buf28, ptr %concat.buf21, i64 %len124)
  %dest229 = getelementptr i8, ptr %concat.buf28, i64 %len124
  %copylen230 = add i64 %len225, 1
  %9 = call ptr @memcpy(ptr %dest229, ptr %alloca14, i64 %copylen230)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf28)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len131 = call i64 @strlen(ptr @str.const.111)
  %len232 = call i64 @strlen(ptr %alloca12)
  %totallen33 = add i64 %len131, %len232
  %bufsize34 = add i64 %totallen33, 1
  %concat.buf35 = alloca i8, i64 %bufsize34, align 1
  %12 = call ptr @memcpy(ptr %concat.buf35, ptr @str.const.111, i64 %len131)
  %dest236 = getelementptr i8, ptr %concat.buf35, i64 %len131
  %copylen237 = add i64 %len232, 1
  %13 = call ptr @memcpy(ptr %dest236, ptr %alloca12, i64 %copylen237)
  %len138 = call i64 @strlen(ptr %concat.buf35)
  %len239 = call i64 @strlen(ptr @str.const.107)
  %totallen40 = add i64 %len138, %len239
  %bufsize41 = add i64 %totallen40, 1
  %concat.buf42 = alloca i8, i64 %bufsize41, align 1
  %14 = call ptr @memcpy(ptr %concat.buf42, ptr %concat.buf35, i64 %len138)
  %dest243 = getelementptr i8, ptr %concat.buf42, i64 %len138
  %copylen244 = add i64 %len239, 1
  %15 = call ptr @memcpy(ptr %dest243, ptr @str.const.107, i64 %copylen244)
  %len145 = call i64 @strlen(ptr %concat.buf42)
  %len246 = call i64 @strlen(ptr %alloca15)
  %totallen47 = add i64 %len145, %len246
  %bufsize48 = add i64 %totallen47, 1
  %concat.buf49 = alloca i8, i64 %bufsize48, align 1
  %16 = call ptr @memcpy(ptr %concat.buf49, ptr %concat.buf42, i64 %len145)
  %dest250 = getelementptr i8, ptr %concat.buf49, i64 %len145
  %copylen251 = add i64 %len246, 1
  %17 = call ptr @memcpy(ptr %dest250, ptr %alloca15, i64 %copylen251)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf49)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len152 = call i64 @strlen(ptr @str.const.116)
  %len253 = call i64 @strlen(ptr %alloca13)
  %totallen54 = add i64 %len152, %len253
  %bufsize55 = add i64 %totallen54, 1
  %concat.buf56 = alloca i8, i64 %bufsize55, align 1
  %20 = call ptr @memcpy(ptr %concat.buf56, ptr @str.const.116, i64 %len152)
  %dest257 = getelementptr i8, ptr %concat.buf56, i64 %len152
  %copylen258 = add i64 %len253, 1
  %21 = call ptr @memcpy(ptr %dest257, ptr %alloca13, i64 %copylen258)
  %len159 = call i64 @strlen(ptr %concat.buf56)
  %len260 = call i64 @strlen(ptr @str.const.107)
  %totallen61 = add i64 %len159, %len260
  %bufsize62 = add i64 %totallen61, 1
  %concat.buf63 = alloca i8, i64 %bufsize62, align 1
  %22 = call ptr @memcpy(ptr %concat.buf63, ptr %concat.buf56, i64 %len159)
  %dest264 = getelementptr i8, ptr %concat.buf63, i64 %len159
  %copylen265 = add i64 %len260, 1
  %23 = call ptr @memcpy(ptr %dest264, ptr @str.const.107, i64 %copylen265)
  %len166 = call i64 @strlen(ptr %concat.buf63)
  %len267 = call i64 @strlen(ptr %alloca16)
  %totallen68 = add i64 %len166, %len267
  %bufsize69 = add i64 %totallen68, 1
  %concat.buf70 = alloca i8, i64 %bufsize69, align 1
  %24 = call ptr @memcpy(ptr %concat.buf70, ptr %concat.buf63, i64 %len166)
  %dest271 = getelementptr i8, ptr %concat.buf70, i64 %len166
  %copylen272 = add i64 %len267, 1
  %25 = call ptr @memcpy(ptr %dest271, ptr %alloca16, i64 %copylen272)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf70)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\85\D9\87\D9\85\D8\A91_\D8\AA\D9\82\D8\AF\D9\85.load73" = load i64, ptr %alloca1, align 8
  %"%\D9\85\D9\87\D9\85\D8\A91_\D9\87\D8\AF\D9\81.load" = load i64, ptr %alloca4, align 8
  %cmpgetmp = icmp sge i64 %"%\D9\85\D9\87\D9\85\D8\A91_\D8\AA\D9\82\D8\AF\D9\85.load73", %"%\D9\85\D9\87\D9\85\D8\A91_\D9\87\D8\AF\D9\81.load"
  br i1 %cmpgetmp, label %then_38, label %else_40

then_38:                                          ; preds = %while_exit_37
  %calltmp = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.121, i64 1)
  br label %merge_39

else_40:                                          ; preds = %while_exit_37
  %calltmp74 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.121, i64 0)
  br label %merge_39

merge_39:                                         ; preds = %else_40, %then_38
  %"%\D9\85\D9\87\D9\85\D8\A92_\D8\AA\D9\82\D8\AF\D9\85.load75" = load i64, ptr %alloca2, align 8
  %"%\D9\85\D9\87\D9\85\D8\A92_\D9\87\D8\AF\D9\81.load" = load i64, ptr %alloca5, align 8
  %cmpgetmp76 = icmp sge i64 %"%\D9\85\D9\87\D9\85\D8\A92_\D8\AA\D9\82\D8\AF\D9\85.load75", %"%\D9\85\D9\87\D9\85\D8\A92_\D9\87\D8\AF\D9\81.load"
  br i1 %cmpgetmp76, label %then_41, label %else_43

then_41:                                          ; preds = %merge_39
  %calltmp77 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.123, i64 1)
  br label %merge_42

else_43:                                          ; preds = %merge_39
  %calltmp78 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.123, i64 0)
  br label %merge_42

merge_42:                                         ; preds = %else_43, %then_41
  %"%\D9\85\D9\87\D9\85\D8\A93_\D8\AA\D9\82\D8\AF\D9\85.load79" = load i64, ptr %alloca3, align 8
  %"%\D9\85\D9\87\D9\85\D8\A93_\D9\87\D8\AF\D9\81.load" = load i64, ptr %alloca6, align 8
  %cmplttmp80 = icmp slt i64 %"%\D9\85\D9\87\D9\85\D8\A93_\D8\AA\D9\82\D8\AF\D9\85.load79", %"%\D9\85\D9\87\D9\85\D8\A93_\D9\87\D8\AF\D9\81.load"
  br i1 %cmplttmp80, label %then_44, label %else_46

then_44:                                          ; preds = %merge_42
  %calltmp81 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.125, i64 1)
  br label %merge_45

else_46:                                          ; preds = %merge_42
  %calltmp82 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.125, i64 0)
  br label %merge_45

merge_45:                                         ; preds = %else_46, %then_44
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.127)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D8\A7\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.131)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.135)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca = alloca i64, align 8
  store i64 7, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 3, ptr %alloca1, align 8
  %alloca2 = alloca i64, align 8
  store i64 1, ptr %alloca2, align 8
  %"%\D9\85\D9\87\D8\A7\D9\85_\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9.load" = load i64, ptr %alloca, align 8
  %"%\D9\85\D9\87\D8\A7\D9\85_\D9\85\D9\86\D8\AA\D8\B8\D8\B1\D8\A9.load" = load i64, ptr %alloca1, align 8
  %addtmp = add i64 %"%\D9\85\D9\87\D8\A7\D9\85_\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9.load", %"%\D9\85\D9\87\D8\A7\D9\85_\D9\85\D9\86\D8\AA\D8\B8\D8\B1\D8\A9.load"
  %"%\D9\85\D9\87\D8\A7\D9\85_\D9\85\D9\84\D8\BA\D8\A7\D8\A9.load" = load i64, ptr %alloca2, align 8
  %addtmp3 = add i64 %addtmp, %"%\D9\85\D9\87\D8\A7\D9\85_\D9\85\D9\84\D8\BA\D8\A7\D8\A9.load"
  %alloca4 = alloca i64, align 8
  store i64 %addtmp3, ptr %alloca4, align 8
  %alloca5 = alloca ptr, align 8
  %alloca6 = alloca ptr, align 8
  %alloca7 = alloca ptr, align 8
  %alloca8 = alloca ptr, align 8
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.139)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.143)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.147)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len1 = call i64 @strlen(ptr @str.const.151)
  %len2 = call i64 @strlen(ptr %alloca5)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %10 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.151, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %11 = call ptr @memcpy(ptr %dest2, ptr %alloca5, i64 %copylen2)
  %len19 = call i64 @strlen(ptr %concat.buf)
  %len210 = call i64 @strlen(ptr @str.const.152)
  %totallen11 = add i64 %len19, %len210
  %bufsize12 = add i64 %totallen11, 1
  %concat.buf13 = alloca i8, i64 %bufsize12, align 1
  %12 = call ptr @memcpy(ptr %concat.buf13, ptr %concat.buf, i64 %len19)
  %dest214 = getelementptr i8, ptr %concat.buf13, i64 %len19
  %copylen215 = add i64 %len210, 1
  %13 = call ptr @memcpy(ptr %dest214, ptr @str.const.152, i64 %copylen215)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf13)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len116 = call i64 @strlen(ptr @str.const.156)
  %len217 = call i64 @strlen(ptr %alloca6)
  %totallen18 = add i64 %len116, %len217
  %bufsize19 = add i64 %totallen18, 1
  %concat.buf20 = alloca i8, i64 %bufsize19, align 1
  %16 = call ptr @memcpy(ptr %concat.buf20, ptr @str.const.156, i64 %len116)
  %dest221 = getelementptr i8, ptr %concat.buf20, i64 %len116
  %copylen222 = add i64 %len217, 1
  %17 = call ptr @memcpy(ptr %dest221, ptr %alloca6, i64 %copylen222)
  %len123 = call i64 @strlen(ptr %concat.buf20)
  %len224 = call i64 @strlen(ptr @str.const.152)
  %totallen25 = add i64 %len123, %len224
  %bufsize26 = add i64 %totallen25, 1
  %concat.buf27 = alloca i8, i64 %bufsize26, align 1
  %18 = call ptr @memcpy(ptr %concat.buf27, ptr %concat.buf20, i64 %len123)
  %dest228 = getelementptr i8, ptr %concat.buf27, i64 %len123
  %copylen229 = add i64 %len224, 1
  %19 = call ptr @memcpy(ptr %dest228, ptr @str.const.152, i64 %copylen229)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf27)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len130 = call i64 @strlen(ptr @str.const.161)
  %len231 = call i64 @strlen(ptr %alloca7)
  %totallen32 = add i64 %len130, %len231
  %bufsize33 = add i64 %totallen32, 1
  %concat.buf34 = alloca i8, i64 %bufsize33, align 1
  %22 = call ptr @memcpy(ptr %concat.buf34, ptr @str.const.161, i64 %len130)
  %dest235 = getelementptr i8, ptr %concat.buf34, i64 %len130
  %copylen236 = add i64 %len231, 1
  %23 = call ptr @memcpy(ptr %dest235, ptr %alloca7, i64 %copylen236)
  %len137 = call i64 @strlen(ptr %concat.buf34)
  %len238 = call i64 @strlen(ptr @str.const.162)
  %totallen39 = add i64 %len137, %len238
  %bufsize40 = add i64 %totallen39, 1
  %concat.buf41 = alloca i8, i64 %bufsize40, align 1
  %24 = call ptr @memcpy(ptr %concat.buf41, ptr %concat.buf34, i64 %len137)
  %dest242 = getelementptr i8, ptr %concat.buf41, i64 %len137
  %copylen243 = add i64 %len238, 1
  %25 = call ptr @memcpy(ptr %dest242, ptr @str.const.162, i64 %copylen243)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf41)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len144 = call i64 @strlen(ptr @str.const.166)
  %len245 = call i64 @strlen(ptr %alloca8)
  %totallen46 = add i64 %len144, %len245
  %bufsize47 = add i64 %totallen46, 1
  %concat.buf48 = alloca i8, i64 %bufsize47, align 1
  %28 = call ptr @memcpy(ptr %concat.buf48, ptr @str.const.166, i64 %len144)
  %dest249 = getelementptr i8, ptr %concat.buf48, i64 %len144
  %copylen250 = add i64 %len245, 1
  %29 = call ptr @memcpy(ptr %dest249, ptr %alloca8, i64 %copylen250)
  %len151 = call i64 @strlen(ptr %concat.buf48)
  %len252 = call i64 @strlen(ptr @str.const.167)
  %totallen53 = add i64 %len151, %len252
  %bufsize54 = add i64 %totallen53, 1
  %concat.buf55 = alloca i8, i64 %bufsize54, align 1
  %30 = call ptr @memcpy(ptr %concat.buf55, ptr %concat.buf48, i64 %len151)
  %dest256 = getelementptr i8, ptr %concat.buf55, i64 %len151
  %copylen257 = add i64 %len252, 1
  %31 = call ptr @memcpy(ptr %dest256, ptr @str.const.167, i64 %copylen257)
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf55)
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\85\D9\87\D8\A7\D9\85_\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9.load58" = load i64, ptr %alloca, align 8
  %multmp = mul i64 %"%\D9\85\D9\87\D8\A7\D9\85_\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9.load58", 100
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load" = load i64, ptr %alloca4, align 8
  %divtmp = sdiv i64 %multmp, %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load"
  %alloca59 = alloca i64, align 8
  store i64 %divtmp, ptr %alloca59, align 8
  %alloca60 = alloca ptr, align 8
  %len161 = call i64 @strlen(ptr @str.const.171)
  %len262 = call i64 @strlen(ptr %alloca60)
  %totallen63 = add i64 %len161, %len262
  %bufsize64 = add i64 %totallen63, 1
  %concat.buf65 = alloca i8, i64 %bufsize64, align 1
  %34 = call ptr @memcpy(ptr %concat.buf65, ptr @str.const.171, i64 %len161)
  %dest266 = getelementptr i8, ptr %concat.buf65, i64 %len161
  %copylen267 = add i64 %len262, 1
  %35 = call ptr @memcpy(ptr %dest266, ptr %alloca60, i64 %copylen267)
  %len168 = call i64 @strlen(ptr %concat.buf65)
  %len269 = call i64 @strlen(ptr @str.const.172)
  %totallen70 = add i64 %len168, %len269
  %bufsize71 = add i64 %totallen70, 1
  %concat.buf72 = alloca i8, i64 %bufsize71, align 1
  %36 = call ptr @memcpy(ptr %concat.buf72, ptr %concat.buf65, i64 %len168)
  %dest273 = getelementptr i8, ptr %concat.buf72, i64 %len168
  %copylen274 = add i64 %len269, 1
  %37 = call ptr @memcpy(ptr %dest273, ptr @str.const.172, i64 %copylen274)
  %38 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf72)
  %39 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %40 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.176)
  %41 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load75" = load i64, ptr %alloca4, align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load75", 11
  br i1 %cmpeqtmp, label %then_47, label %else_49

then_47:                                          ; preds = %entry
  %calltmp = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.180, i64 1)
  br label %merge_48

else_49:                                          ; preds = %entry
  %calltmp76 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.180, i64 0)
  br label %merge_48

merge_48:                                         ; preds = %else_49, %then_47
  %"%\D9\86\D8\B3\D8\A8\D8\A9.load" = load i64, ptr %alloca59, align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\86\D8\B3\D8\A8\D8\A9.load", 60
  br i1 %cmpgttmp, label %then_50, label %else_52

then_50:                                          ; preds = %merge_48
  %calltmp77 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.182, i64 1)
  br label %merge_51

else_52:                                          ; preds = %merge_48
  %calltmp78 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.182, i64 0)
  br label %merge_51

merge_51:                                         ; preds = %else_52, %then_50
  %42 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.184)
  %43 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.188)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.192)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.196)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.200)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D9\85\D9\87\D8\A7\D9\85_\D8\A8\D8\B3\D9\8A\D8\B7\D8\A9"()
  %calltmp1 = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D8\AC\D8\AF\D9\88\D9\84\D8\A9"()
  %calltmp2 = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D8\AA\D9\86\D8\A7\D9\81\D8\B3"()
  %calltmp3 = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D8\A7\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA"()
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.204)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
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
