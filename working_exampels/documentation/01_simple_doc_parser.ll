; ModuleID = '01_simple_doc_parser'
source_filename = "01_simple_doc_parser"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [9 x i8] c"  [\E2\9C\93] \00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.1 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.3 = private unnamed_addr constant [9 x i8] c"  [\E2\9C\97] \00", align 1
@str.ret = private unnamed_addr constant [16 x i8] c"\D9\88\D8\B5\D9\81_\D8\B7\D9\88\D9\8A\D9\84\00", align 1
@str.ret.7 = private unnamed_addr constant [16 x i8] c"\D9\88\D8\B5\D9\81_\D9\82\D8\B5\D9\8A\D8\B1\00", align 1
@str.ret.8 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.9 = private unnamed_addr constant [45 x i8] c"=== \D8\AA\D8\AD\D9\84\D9\8A\D9\84 \D8\AA\D8\B9\D9\84\D9\8A\D9\82\D8\A7\D8\AA \D8\A8\D8\B3\D9\8A\D8\B7\D8\A9 ===\00", align 1
@str.const.13 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.17 = private unnamed_addr constant [46 x i8] c"/// \D9\87\D8\B0\D9\87 \D8\AF\D8\A7\D9\84\D8\A9 \D9\84\D8\AD\D8\B3\D8\A7\D8\A8 \D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\00", align 1
@str.const.18 = private unnamed_addr constant [44 x i8] c"\D8\A7\D9\84\D8\AA\D8\B9\D8\B1\D9\81 \D8\B9\D9\84\D9\89 \D8\AA\D8\B9\D9\84\D9\8A\D9\82 \D8\AA\D9\88\D8\AB\D9\8A\D9\82\D9\8A\00", align 1
@str.const.19 = private unnamed_addr constant [41 x i8] c"\D8\A7\D8\B3\D8\AA\D8\AE\D9\84\D8\A7\D8\B5 \D9\85\D8\AD\D8\AA\D9\88\D9\89 \D8\A7\D9\84\D8\AA\D8\B9\D9\84\D9\8A\D9\82\00", align 1
@str.const.21 = private unnamed_addr constant [17 x i8] c"    \D8\A7\D9\84\D9\86\D9\88\D8\B9: \00", align 1
@str.const.25 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.29 = private unnamed_addr constant [17 x i8] c"  \D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA: \00", align 1
@str.const.30 = private unnamed_addr constant [3 x i8] c"/3\00", align 1
@str.const.33 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.37 = private unnamed_addr constant [47 x i8] c"=== \D8\AA\D8\AD\D9\84\D9\8A\D9\84 \D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9 \D8\AA\D8\B9\D9\84\D9\8A\D9\82\D8\A7\D8\AA ===\00", align 1
@str.const.41 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.45 = private unnamed_addr constant [41 x i8] c"/// \D9\88\D8\B5\D9\81 \D8\A7\D9\84\D8\AF\D8\A7\D9\84\D8\A9 \D8\A7\D9\84\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9\00", align 1
@str.const.46 = private unnamed_addr constant [56 x i8] c"/// \D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\85\D9\84\D8\A7\D8\AA: \D8\B9\D8\AF\D8\AF1 - \D8\A7\D9\84\D8\B1\D9\82\D9\85 \D8\A7\D9\84\D8\A3\D9\88\D9\84\00", align 1
@str.const.47 = private unnamed_addr constant [58 x i8] c"/// \D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\85\D9\84\D8\A7\D8\AA: \D8\B9\D8\AF\D8\AF2 - \D8\A7\D9\84\D8\B1\D9\82\D9\85 \D8\A7\D9\84\D8\AB\D8\A7\D9\86\D9\8A\00", align 1
@str.const.48 = private unnamed_addr constant [46 x i8] c"/// \D8\A7\D9\84\D8\A5\D8\B1\D8\AC\D8\A7\D8\B9: \D9\85\D8\AC\D9\85\D9\88\D8\B9 \D8\A7\D9\84\D8\B9\D8\AF\D8\AF\D9\8A\D9\86\00", align 1
@str.const.49 = private unnamed_addr constant [22 x i8] c"# \D8\AA\D8\B9\D9\84\D9\8A\D9\82 \D8\B9\D8\A7\D8\AF\D9\8A\00", align 1
@str.const.50 = private unnamed_addr constant [36 x i8] c"  \D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A \D8\A7\D9\84\D8\AA\D8\B9\D9\84\D9\8A\D9\82\D8\A7\D8\AA: \00", align 1
@str.const.53 = private unnamed_addr constant [34 x i8] c"  \D8\AA\D8\B9\D9\84\D9\8A\D9\82\D8\A7\D8\AA \D8\AA\D9\88\D8\AB\D9\8A\D9\82\D9\8A\D8\A9: \00", align 1
@str.const.56 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.60 = private unnamed_addr constant [35 x i8] c"  \D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\84 \D8\A7\D9\84\D8\AA\D8\B9\D9\84\D9\8A\D9\82\D8\A7\D8\AA:\00", align 1
@str.const.64 = private unnamed_addr constant [8 x i8] c"    1: \00", align 1
@str.const.65 = private unnamed_addr constant [5 x i8] c" -> \00", align 1
@str.const.69 = private unnamed_addr constant [8 x i8] c"    2: \00", align 1
@str.const.74 = private unnamed_addr constant [8 x i8] c"    4: \00", align 1
@str.const.79 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.83 = private unnamed_addr constant [39 x i8] c"\D8\AA\D8\AD\D9\84\D9\8A\D9\84 \D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9 \D8\A7\D9\83\D8\AA\D9\85\D9\84\00", align 1
@str.const.84 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.88 = private unnamed_addr constant [43 x i8] c"=== \D8\AA\D9\88\D9\84\D9\8A\D8\AF \D9\85\D9\84\D8\AE\D8\B5 \D8\A7\D9\84\D8\AA\D9\88\D8\AB\D9\8A\D9\82 ===\00", align 1
@str.const.92 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.96 = private unnamed_addr constant [87 x i8] c"  \E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.100 = private unnamed_addr constant [45 x i8] c"  \E2\95\91 \D9\85\D9\84\D8\AE\D8\B5 \D8\A7\D9\84\D8\AA\D9\88\D8\AB\D9\8A\D9\82            \E2\95\91\00", align 1
@str.const.104 = private unnamed_addr constant [87 x i8] c"  \E2\95\A0\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\A3\00", align 1
@str.const.108 = private unnamed_addr constant [48 x i8] c"  \E2\95\91 \D8\A7\D9\84\D8\AF\D9\88\D8\A7\D9\84 \D8\A7\D9\84\D9\85\D9\88\D8\AB\D9\82\D8\A9: 3        \E2\95\91\00", align 1
@str.const.112 = private unnamed_addr constant [44 x i8] c"  \E2\95\91 \D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\85\D9\84\D8\A7\D8\AA: 5             \E2\95\91\00", align 1
@str.const.116 = private unnamed_addr constant [44 x i8] c"  \E2\95\91 \D9\82\D9\8A\D9\85 \D8\A7\D9\84\D8\A5\D8\B1\D8\AC\D8\A7\D8\B9: 3          \E2\95\91\00", align 1
@str.const.120 = private unnamed_addr constant [45 x i8] c"  \E2\95\91 \D9\86\D8\B3\D8\A8\D8\A9 \D8\A7\D9\84\D8\AA\D8\BA\D8\B7\D9\8A\D8\A9: 85%       \E2\95\91\00", align 1
@str.const.124 = private unnamed_addr constant [87 x i8] c"  \E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.128 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.132 = private unnamed_addr constant [35 x i8] c"\D8\AA\D9\88\D9\84\D9\8A\D8\AF \D8\A7\D9\84\D9\85\D9\84\D8\AE\D8\B5 \D8\A7\D9\83\D8\AA\D9\85\D9\84\00", align 1
@str.const.133 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.137 = private unnamed_addr constant [112 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.141 = private unnamed_addr constant [64 x i8] c"\E2\95\91  \D9\85\D8\AD\D9\84\D9\84 \D8\A7\D9\84\D8\AA\D8\B9\D9\84\D9\8A\D9\82\D8\A7\D8\AA \D8\A7\D9\84\D8\AA\D9\88\D8\AB\D9\8A\D9\82\D9\8A\D8\A9         \E2\95\91\00", align 1
@str.const.145 = private unnamed_addr constant [112 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.149 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.153 = private unnamed_addr constant [45 x i8] c"=== \D8\A7\D9\86\D8\AA\D9\87\D9\89 \D8\A7\D9\84\D8\AA\D8\AD\D9\84\D9\8A\D9\84 \D8\A8\D9\86\D8\AC\D8\A7\D8\AD ===\00", align 1

define i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr %"\D9\88\D8\B5\D9\81", i64 %"\D8\AD\D8\A7\D9\84\D8\AA\D9\87") {
entry:
  %"\D8\AD\D8\A7\D9\84\D8\AA\D9\872" = alloca i64, align 8
  store i64 %"\D8\AD\D8\A7\D9\84\D8\AA\D9\87", ptr %"\D8\AD\D8\A7\D9\84\D8\AA\D9\872", align 8
  %"\D9\88\D8\B5\D9\811" = alloca ptr, align 8
  store ptr %"\D9\88\D8\B5\D9\81", ptr %"\D9\88\D8\B5\D9\811", align 8
  %"%\D8\AD\D8\A7\D9\84\D8\AA\D9\87.load" = load i64, ptr %"\D8\AD\D8\A7\D9\84\D8\AA\D9\872", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\AD\D8\A7\D9\84\D8\AA\D9\87.load", 1
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

define i64 @"\D9\87\D9\84_\D8\AA\D8\B9\D9\84\D9\8A\D9\82_\D8\AA\D9\88\D8\AB\D9\8A\D9\82\D9\8A"(ptr %"\D8\B3\D8\B7\D8\B1") {
entry:
  %"\D8\B3\D8\B7\D8\B11" = alloca ptr, align 8
  store ptr %"\D8\B3\D8\B7\D8\B1", ptr %"\D8\B3\D8\B7\D8\B11", align 8
  %strlen.ret = call i64 @strlen(ptr %"\D8\B3\D8\B7\D8\B11")
  %alloca = alloca i64, align 8
  store i64 %strlen.ret, ptr %alloca, align 8
  %"%\D8\B7\D9\88\D9\84.load" = load i64, ptr %alloca, align 8
  %cmplttmp = icmp slt i64 %"%\D8\B7\D9\88\D9\84.load", 3
  br i1 %cmplttmp, label %then_3, label %merge_4

then_3:                                           ; preds = %entry
  ret i64 0

merge_4:                                          ; preds = %entry
  ret i64 1
}

define ptr @"\D8\A7\D8\B3\D8\AA\D8\AE\D9\84\D8\A7\D8\B5_\D9\85\D8\AD\D8\AA\D9\88\D9\89"(ptr %"\D8\AA\D8\B9\D9\84\D9\8A\D9\82") {
entry:
  %"\D8\AA\D8\B9\D9\84\D9\8A\D9\821" = alloca ptr, align 8
  store ptr %"\D8\AA\D8\B9\D9\84\D9\8A\D9\82", ptr %"\D8\AA\D8\B9\D9\84\D9\8A\D9\821", align 8
  %loadtmp = load ptr, ptr %"\D8\AA\D8\B9\D9\84\D9\8A\D9\821", align 8
  ret ptr %loadtmp
}

define ptr @"\D8\AA\D8\AD\D8\AF\D9\8A\D8\AF_\D9\86\D9\88\D8\B9_\D8\AA\D8\B9\D9\84\D9\8A\D9\82"(ptr %"\D9\85\D8\AD\D8\AA\D9\88\D9\89") {
entry:
  %"\D9\85\D8\AD\D8\AA\D9\88\D9\891" = alloca ptr, align 8
  store ptr %"\D9\85\D8\AD\D8\AA\D9\88\D9\89", ptr %"\D9\85\D8\AD\D8\AA\D9\88\D9\891", align 8
  %strlen.ret = call i64 @strlen(ptr %"\D9\85\D8\AD\D8\AA\D9\88\D9\891")
  %alloca = alloca i64, align 8
  store i64 %strlen.ret, ptr %alloca, align 8
  %"%\D8\B7\D9\88\D9\84.load" = load i64, ptr %alloca, align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B7\D9\88\D9\84.load", 10
  br i1 %cmpgttmp, label %then_5, label %else_7

then_5:                                           ; preds = %entry
  ret ptr @str.ret

else_7:                                           ; preds = %entry
  ret ptr @str.ret.7

merge_6:                                          ; No predecessors!
  ret ptr @str.ret.8
}

define i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D8\AA\D8\B9\D9\84\D9\8A\D9\82\D8\A7\D8\AA_\D8\A8\D8\B3\D9\8A\D8\B7\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.9)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.13)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %alloca1 = alloca ptr, align 8
  store ptr @str.const.17, ptr %alloca1, align 8
  %calltmp = call i64 @"\D9\87\D9\84_\D8\AA\D8\B9\D9\84\D9\8A\D9\82_\D8\AA\D9\88\D8\AB\D9\8A\D9\82\D9\8A"(ptr %alloca1)
  %alloca2 = alloca i64, align 8
  store i64 %calltmp, ptr %alloca2, align 8
  %"%\D9\881.load" = load i64, ptr %alloca2, align 8
  %calltmp3 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.18, i64 %"%\D9\881.load")
  %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load" = load i64, ptr %alloca, align 8
  %"%\D9\881.load4" = load i64, ptr %alloca2, align 8
  %addtmp = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load", %"%\D9\881.load4"
  store i64 %addtmp, ptr %alloca, align 8
  %calltmp5 = call ptr @"\D8\A7\D8\B3\D8\AA\D8\AE\D9\84\D8\A7\D8\B5_\D9\85\D8\AD\D8\AA\D9\88\D9\89"(ptr %alloca1)
  %alloca6 = alloca ptr, align 8
  store ptr %calltmp5, ptr %alloca6, align 8
  %strlen.ret = call i64 @strlen(ptr %alloca6)
  %alloca7 = alloca i64, align 8
  store i64 %strlen.ret, ptr %alloca7, align 8
  %"%\D8\B71.load" = load i64, ptr %alloca7, align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B71.load", 0
  br i1 %cmpgttmp, label %then_8, label %else_10

then_8:                                           ; preds = %entry
  %calltmp8 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.19, i64 1)
  %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load9" = load i64, ptr %alloca, align 8
  %addtmp10 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load9", 1
  store i64 %addtmp10, ptr %alloca, align 8
  br label %merge_9

else_10:                                          ; preds = %entry
  %calltmp11 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.19, i64 0)
  br label %merge_9

merge_9:                                          ; preds = %else_10, %then_8
  %calltmp12 = call ptr @"\D8\AA\D8\AD\D8\AF\D9\8A\D8\AF_\D9\86\D9\88\D8\B9_\D8\AA\D8\B9\D9\84\D9\8A\D9\82"(ptr %alloca6)
  %alloca13 = alloca ptr, align 8
  store ptr %calltmp12, ptr %alloca13, align 8
  %len1 = call i64 @strlen(ptr @str.const.21)
  %len2 = call i64 @strlen(ptr %alloca13)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %4 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.21, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %5 = call ptr @memcpy(ptr %dest2, ptr %alloca13, i64 %copylen2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load14" = load i64, ptr %alloca, align 8
  %addtmp15 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load14", 1
  store i64 %addtmp15, ptr %alloca, align 8
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.25)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.33)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9_\D8\AA\D8\B9\D9\84\D9\8A\D9\82\D8\A7\D8\AA"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.37)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.41)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 0, ptr %alloca1, align 8
  %alloca2 = alloca ptr, align 8
  store ptr @str.const.45, ptr %alloca2, align 8
  %alloca3 = alloca ptr, align 8
  store ptr @str.const.46, ptr %alloca3, align 8
  %alloca4 = alloca ptr, align 8
  store ptr @str.const.47, ptr %alloca4, align 8
  %alloca5 = alloca ptr, align 8
  store ptr @str.const.48, ptr %alloca5, align 8
  %alloca6 = alloca ptr, align 8
  store ptr @str.const.49, ptr %alloca6, align 8
  store i64 5, ptr %alloca, align 8
  %calltmp = call i64 @"\D9\87\D9\84_\D8\AA\D8\B9\D9\84\D9\8A\D9\82_\D8\AA\D9\88\D8\AB\D9\8A\D9\82\D9\8A"(ptr %alloca2)
  %alloca7 = alloca i64, align 8
  store i64 %calltmp, ptr %alloca7, align 8
  %calltmp8 = call i64 @"\D9\87\D9\84_\D8\AA\D8\B9\D9\84\D9\8A\D9\82_\D8\AA\D9\88\D8\AB\D9\8A\D9\82\D9\8A"(ptr %alloca3)
  %alloca9 = alloca i64, align 8
  store i64 %calltmp8, ptr %alloca9, align 8
  %calltmp10 = call i64 @"\D9\87\D9\84_\D8\AA\D8\B9\D9\84\D9\8A\D9\82_\D8\AA\D9\88\D8\AB\D9\8A\D9\82\D9\8A"(ptr %alloca4)
  %alloca11 = alloca i64, align 8
  store i64 %calltmp10, ptr %alloca11, align 8
  %calltmp12 = call i64 @"\D9\87\D9\84_\D8\AA\D8\B9\D9\84\D9\8A\D9\82_\D8\AA\D9\88\D8\AB\D9\8A\D9\82\D9\8A"(ptr %alloca5)
  %alloca13 = alloca i64, align 8
  store i64 %calltmp12, ptr %alloca13, align 8
  %calltmp14 = call i64 @"\D9\87\D9\84_\D8\AA\D8\B9\D9\84\D9\8A\D9\82_\D8\AA\D9\88\D8\AB\D9\8A\D9\82\D9\8A"(ptr %alloca6)
  %alloca15 = alloca i64, align 8
  store i64 %calltmp14, ptr %alloca15, align 8
  %"%\D9\861.load" = load i64, ptr %alloca7, align 8
  %"%\D9\862.load" = load i64, ptr %alloca9, align 8
  %addtmp = add i64 %"%\D9\861.load", %"%\D9\862.load"
  %"%\D9\863.load" = load i64, ptr %alloca11, align 8
  %addtmp16 = add i64 %addtmp, %"%\D9\863.load"
  %"%\D9\864.load" = load i64, ptr %alloca13, align 8
  %addtmp17 = add i64 %addtmp16, %"%\D9\864.load"
  %"%\D9\865.load" = load i64, ptr %alloca15, align 8
  %addtmp18 = add i64 %addtmp17, %"%\D9\865.load"
  store i64 %addtmp18, ptr %alloca1, align 8
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.56)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.60)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp19 = call ptr @"\D8\AA\D8\AD\D8\AF\D9\8A\D8\AF_\D9\86\D9\88\D8\B9_\D8\AA\D8\B9\D9\84\D9\8A\D9\82"(ptr %alloca2)
  %alloca20 = alloca ptr, align 8
  store ptr %calltmp19, ptr %alloca20, align 8
  %len1 = call i64 @strlen(ptr @str.const.64)
  %len2 = call i64 @strlen(ptr %alloca20)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %10 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.64, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %11 = call ptr @memcpy(ptr %dest2, ptr %alloca20, i64 %copylen2)
  %len121 = call i64 @strlen(ptr %concat.buf)
  %len222 = call i64 @strlen(ptr @str.const.65)
  %totallen23 = add i64 %len121, %len222
  %bufsize24 = add i64 %totallen23, 1
  %concat.buf25 = alloca i8, i64 %bufsize24, align 1
  %12 = call ptr @memcpy(ptr %concat.buf25, ptr %concat.buf, i64 %len121)
  %dest226 = getelementptr i8, ptr %concat.buf25, i64 %len121
  %copylen227 = add i64 %len222, 1
  %13 = call ptr @memcpy(ptr %dest226, ptr @str.const.65, i64 %copylen227)
  %len128 = call i64 @strlen(ptr %concat.buf25)
  %len229 = call i64 @strlen(ptr %alloca2)
  %totallen30 = add i64 %len128, %len229
  %bufsize31 = add i64 %totallen30, 1
  %concat.buf32 = alloca i8, i64 %bufsize31, align 1
  %14 = call ptr @memcpy(ptr %concat.buf32, ptr %concat.buf25, i64 %len128)
  %dest233 = getelementptr i8, ptr %concat.buf32, i64 %len128
  %copylen234 = add i64 %len229, 1
  %15 = call ptr @memcpy(ptr %dest233, ptr %alloca2, i64 %copylen234)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf32)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp35 = call ptr @"\D8\AA\D8\AD\D8\AF\D9\8A\D8\AF_\D9\86\D9\88\D8\B9_\D8\AA\D8\B9\D9\84\D9\8A\D9\82"(ptr %alloca3)
  %alloca36 = alloca ptr, align 8
  store ptr %calltmp35, ptr %alloca36, align 8
  %len137 = call i64 @strlen(ptr @str.const.69)
  %len238 = call i64 @strlen(ptr %alloca36)
  %totallen39 = add i64 %len137, %len238
  %bufsize40 = add i64 %totallen39, 1
  %concat.buf41 = alloca i8, i64 %bufsize40, align 1
  %18 = call ptr @memcpy(ptr %concat.buf41, ptr @str.const.69, i64 %len137)
  %dest242 = getelementptr i8, ptr %concat.buf41, i64 %len137
  %copylen243 = add i64 %len238, 1
  %19 = call ptr @memcpy(ptr %dest242, ptr %alloca36, i64 %copylen243)
  %len144 = call i64 @strlen(ptr %concat.buf41)
  %len245 = call i64 @strlen(ptr @str.const.65)
  %totallen46 = add i64 %len144, %len245
  %bufsize47 = add i64 %totallen46, 1
  %concat.buf48 = alloca i8, i64 %bufsize47, align 1
  %20 = call ptr @memcpy(ptr %concat.buf48, ptr %concat.buf41, i64 %len144)
  %dest249 = getelementptr i8, ptr %concat.buf48, i64 %len144
  %copylen250 = add i64 %len245, 1
  %21 = call ptr @memcpy(ptr %dest249, ptr @str.const.65, i64 %copylen250)
  %len151 = call i64 @strlen(ptr %concat.buf48)
  %len252 = call i64 @strlen(ptr %alloca3)
  %totallen53 = add i64 %len151, %len252
  %bufsize54 = add i64 %totallen53, 1
  %concat.buf55 = alloca i8, i64 %bufsize54, align 1
  %22 = call ptr @memcpy(ptr %concat.buf55, ptr %concat.buf48, i64 %len151)
  %dest256 = getelementptr i8, ptr %concat.buf55, i64 %len151
  %copylen257 = add i64 %len252, 1
  %23 = call ptr @memcpy(ptr %dest256, ptr %alloca3, i64 %copylen257)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf55)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp58 = call ptr @"\D8\AA\D8\AD\D8\AF\D9\8A\D8\AF_\D9\86\D9\88\D8\B9_\D8\AA\D8\B9\D9\84\D9\8A\D9\82"(ptr %alloca5)
  %alloca59 = alloca ptr, align 8
  store ptr %calltmp58, ptr %alloca59, align 8
  %len160 = call i64 @strlen(ptr @str.const.74)
  %len261 = call i64 @strlen(ptr %alloca59)
  %totallen62 = add i64 %len160, %len261
  %bufsize63 = add i64 %totallen62, 1
  %concat.buf64 = alloca i8, i64 %bufsize63, align 1
  %26 = call ptr @memcpy(ptr %concat.buf64, ptr @str.const.74, i64 %len160)
  %dest265 = getelementptr i8, ptr %concat.buf64, i64 %len160
  %copylen266 = add i64 %len261, 1
  %27 = call ptr @memcpy(ptr %dest265, ptr %alloca59, i64 %copylen266)
  %len167 = call i64 @strlen(ptr %concat.buf64)
  %len268 = call i64 @strlen(ptr @str.const.65)
  %totallen69 = add i64 %len167, %len268
  %bufsize70 = add i64 %totallen69, 1
  %concat.buf71 = alloca i8, i64 %bufsize70, align 1
  %28 = call ptr @memcpy(ptr %concat.buf71, ptr %concat.buf64, i64 %len167)
  %dest272 = getelementptr i8, ptr %concat.buf71, i64 %len167
  %copylen273 = add i64 %len268, 1
  %29 = call ptr @memcpy(ptr %dest272, ptr @str.const.65, i64 %copylen273)
  %len174 = call i64 @strlen(ptr %concat.buf71)
  %len275 = call i64 @strlen(ptr %alloca5)
  %totallen76 = add i64 %len174, %len275
  %bufsize77 = add i64 %totallen76, 1
  %concat.buf78 = alloca i8, i64 %bufsize77, align 1
  %30 = call ptr @memcpy(ptr %concat.buf78, ptr %concat.buf71, i64 %len174)
  %dest279 = getelementptr i8, ptr %concat.buf78, i64 %len174
  %copylen280 = add i64 %len275, 1
  %31 = call ptr @memcpy(ptr %dest279, ptr %alloca5, i64 %copylen280)
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf78)
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %34 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.79)
  %35 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp81 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.83, i64 1)
  %36 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.84)
  %37 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D8\AA\D9\88\D9\84\D9\8A\D8\AF_\D9\85\D9\84\D8\AE\D8\B5"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.88)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.92)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.96)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.100)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.104)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.108)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.112)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.116)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.120)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.124)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.128)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.132, i64 1)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.133)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.137)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.141)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.145)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.149)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D8\AA\D8\B9\D9\84\D9\8A\D9\82\D8\A7\D8\AA_\D8\A8\D8\B3\D9\8A\D8\B7\D8\A9"()
  %calltmp1 = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9_\D8\AA\D8\B9\D9\84\D9\8A\D9\82\D8\A7\D8\AA"()
  %calltmp2 = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D8\AA\D9\88\D9\84\D9\8A\D8\AF_\D9\85\D9\84\D8\AE\D8\B5"()
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.153)
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
