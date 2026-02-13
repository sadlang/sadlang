; ModuleID = '02_simple_html_generator'
source_filename = "02_simple_html_generator"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [2 x i8] c"<\00", align 1
@str.const.1 = private unnamed_addr constant [2 x i8] c">\00", align 1
@str.const.2 = private unnamed_addr constant [3 x i8] c"</\00", align 1
@str.const.4 = private unnamed_addr constant [9 x i8] c"  [\E2\9C\93] \00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.5 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.7 = private unnamed_addr constant [9 x i8] c"  [\E2\9C\97] \00", align 1
@str.const.11 = private unnamed_addr constant [5 x i8] c"    \00", align 1
@str.ret = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.ret.17 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.18 = private unnamed_addr constant [3 x i8] c"em\00", align 1
@str.const.19 = private unnamed_addr constant [3 x i8] c" (\00", align 1
@str.const.20 = private unnamed_addr constant [4 x i8] c"): \00", align 1
@str.const.21 = private unnamed_addr constant [3 x i8] c"li\00", align 1
@str.const.22 = private unnamed_addr constant [7 x i8] c"      \00", align 1
@str.ret.26 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.27 = private unnamed_addr constant [17 x i8] c"\D8\A7\D9\84\D8\A5\D8\B1\D8\AC\D8\A7\D8\B9: \00", align 1
@str.const.28 = private unnamed_addr constant [4 x i8] c" - \00", align 1
@str.const.29 = private unnamed_addr constant [2 x i8] c"p\00", align 1
@str.ret.34 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.35 = private unnamed_addr constant [41 x i8] c"=== \D8\AA\D9\88\D9\84\D9\8A\D8\AF \D9\88\D8\AB\D9\8A\D9\82\D8\A9 \D8\A8\D8\B3\D9\8A\D8\B7\D8\A9 ===\00", align 1
@str.const.39 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.43 = private unnamed_addr constant [27 x i8] c"  HTML \D8\A7\D9\84\D9\85\D9\8F\D9\88\D9\84\D9\8E\D9\91\D8\AF:\00", align 1
@str.const.47 = private unnamed_addr constant [42 x i8] c"  \E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\00", align 1
@str.const.51 = private unnamed_addr constant [33 x i8] c"\D8\AF\D8\A7\D9\84\D8\A9 \D8\A7\D8\AD\D8\B3\D8\A8_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\00", align 1
@str.const.52 = private unnamed_addr constant [44 x i8] c"\D8\AA\D8\AD\D8\B3\D8\A8 \D9\85\D8\AC\D9\85\D9\88\D8\B9 \D8\B9\D8\AF\D8\AF\D9\8A\D9\86 \D8\B5\D8\AD\D9\8A\D8\AD\D9\8A\D9\86\00", align 1
@str.const.53 = private unnamed_addr constant [9 x i8] c"    <ul>\00", align 1
@str.const.57 = private unnamed_addr constant [8 x i8] c"\D8\B9\D8\AF\D8\AF1\00", align 1
@str.const.58 = private unnamed_addr constant [16 x i8] c"\D8\B9\D8\AF\D8\AF_\D8\B5\D8\AD\D9\8A\D8\AD\00", align 1
@str.const.59 = private unnamed_addr constant [22 x i8] c"\D8\A7\D9\84\D8\B9\D8\AF\D8\AF \D8\A7\D9\84\D8\A3\D9\88\D9\84\00", align 1
@str.const.60 = private unnamed_addr constant [8 x i8] c"\D8\B9\D8\AF\D8\AF2\00", align 1
@str.const.62 = private unnamed_addr constant [24 x i8] c"\D8\A7\D9\84\D8\B9\D8\AF\D8\AF \D8\A7\D9\84\D8\AB\D8\A7\D9\86\D9\8A\00", align 1
@str.const.63 = private unnamed_addr constant [10 x i8] c"    </ul>\00", align 1
@str.const.68 = private unnamed_addr constant [26 x i8] c"\D9\85\D8\AC\D9\85\D9\88\D8\B9 \D8\A7\D9\84\D8\B9\D8\AF\D8\AF\D9\8A\D9\86\00", align 1
@str.const.69 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.73 = private unnamed_addr constant [41 x i8] c"\D8\AA\D9\88\D9\84\D9\8A\D8\AF \D8\A7\D9\84\D9\88\D8\AB\D9\8A\D9\82\D8\A9 \D8\A7\D9\84\D8\A8\D8\B3\D9\8A\D8\B7\D8\A9\00", align 1
@str.const.74 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.78 = private unnamed_addr constant [43 x i8] c"=== \D8\AA\D9\88\D9\84\D9\8A\D8\AF \D9\81\D9\87\D8\B1\D8\B3 \D8\A7\D9\84\D9\88\D8\AB\D8\A7\D8\A6\D9\82 ===\00", align 1
@str.const.82 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.86 = private unnamed_addr constant [21 x i8] c"  HTML \D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3:\00", align 1
@str.const.90 = private unnamed_addr constant [39 x i8] c"  \E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\00", align 1
@str.const.94 = private unnamed_addr constant [3 x i8] c"h1\00", align 1
@str.const.95 = private unnamed_addr constant [41 x i8] c"\D8\AA\D9\88\D8\AB\D9\8A\D9\82 \D9\85\D9\83\D8\AA\D8\A8\D8\A9 \D8\A7\D9\84\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA\00", align 1
@str.const.96 = private unnamed_addr constant [3 x i8] c"  \00", align 1
@str.const.100 = private unnamed_addr constant [8 x i8] c"  <nav>\00", align 1
@str.const.108 = private unnamed_addr constant [2 x i8] c"a\00", align 1
@str.const.109 = private unnamed_addr constant [24 x i8] c"\D8\A7\D8\AD\D8\B3\D8\A8_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\00", align 1
@str.const.111 = private unnamed_addr constant [20 x i8] c"\D8\A7\D8\AD\D8\B3\D8\A8_\D8\A7\D9\84\D9\81\D8\B1\D9\82\00", align 1
@str.const.113 = private unnamed_addr constant [20 x i8] c"\D8\A7\D8\AD\D8\B3\D8\A8_\D8\A7\D9\84\D8\B6\D8\B1\D8\A8\00", align 1
@str.const.115 = private unnamed_addr constant [22 x i8] c"\D8\A7\D8\AD\D8\B3\D8\A8_\D8\A7\D9\84\D9\82\D8\B3\D9\85\D8\A9\00", align 1
@str.const.140 = private unnamed_addr constant [9 x i8] c"  </nav>\00", align 1
@str.const.144 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.148 = private unnamed_addr constant [42 x i8] c"  \D8\B9\D8\AF\D8\AF \D8\A7\D9\84\D8\AF\D9\88\D8\A7\D9\84 \D9\81\D9\8A \D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3: \00", align 1
@str.const.152 = private unnamed_addr constant [24 x i8] c"\D8\AA\D9\88\D9\84\D9\8A\D8\AF \D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3\00", align 1
@str.const.153 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.157 = private unnamed_addr constant [46 x i8] c"=== \D8\A7\D9\84\D8\AA\D8\AD\D9\82\D9\82 \D9\85\D9\86 \D8\B5\D8\AD\D8\A9 \D8\A7\D9\84\D9\88\D8\B3\D9\88\D9\85 ===\00", align 1
@str.const.161 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.165 = private unnamed_addr constant [4 x i8] c"div\00", align 1
@str.const.166 = private unnamed_addr constant [11 x i8] c"\D9\85\D8\AD\D8\AA\D9\88\D9\89\00", align 1
@str.const.167 = private unnamed_addr constant [20 x i8] c"\D9\88\D8\B3\D9\85 div \D8\B5\D8\AD\D9\8A\D8\AD\00", align 1
@str.const.169 = private unnamed_addr constant [5 x i8] c"span\00", align 1
@str.const.170 = private unnamed_addr constant [20 x i8] c"    \D9\88\D8\B3\D9\85 \D9\81\D8\AA\D8\AD: \00", align 1
@str.const.175 = private unnamed_addr constant [20 x i8] c"    \D9\88\D8\B3\D9\85 \D8\BA\D9\84\D9\82: \00", align 1
@str.const.179 = private unnamed_addr constant [7 x i8] c"strong\00", align 1
@str.const.180 = private unnamed_addr constant [7 x i8] c"\D9\85\D9\87\D9\85\00", align 1
@str.const.182 = private unnamed_addr constant [19 x i8] c"    \D9\85\D8\AA\D8\AF\D8\A7\D8\AE\D9\84: \00", align 1
@str.const.186 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.190 = private unnamed_addr constant [17 x i8] c"  \D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA: \00", align 1
@str.const.191 = private unnamed_addr constant [3 x i8] c"/4\00", align 1
@str.const.195 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.199 = private unnamed_addr constant [112 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.203 = private unnamed_addr constant [60 x i8] c"\E2\95\91  \D9\85\D9\88\D9\84\D8\AF \D8\B5\D9\81\D8\AD\D8\A7\D8\AA HTML \D9\85\D9\86 \D8\A7\D9\84\D8\AA\D9\88\D8\AB\D9\8A\D9\82       \E2\95\91\00", align 1
@str.const.207 = private unnamed_addr constant [112 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.211 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.215 = private unnamed_addr constant [45 x i8] c"=== \D8\A7\D9\86\D8\AA\D9\87\D9\89 \D8\A7\D9\84\D8\AA\D9\88\D9\84\D9\8A\D8\AF \D8\A8\D9\86\D8\AC\D8\A7\D8\AD ===\00", align 1

define ptr @"\D9\88\D8\B3\D9\85_\D9\81\D8\AA\D8\AD"(ptr %"\D8\A7\D8\B3\D9\85_\D9\88") {
entry:
  %"\D8\A7\D8\B3\D9\85_\D9\881" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85_\D9\88", ptr %"\D8\A7\D8\B3\D9\85_\D9\881", align 8
  %len1 = call i64 @strlen(ptr @str.const)
  %len2 = call i64 @strlen(ptr %"\D8\A7\D8\B3\D9\85_\D9\881")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\A7\D8\B3\D9\85_\D9\881", i64 %copylen2)
  %len12 = call i64 @strlen(ptr %concat.buf)
  %len23 = call i64 @strlen(ptr @str.const.1)
  %totallen4 = add i64 %len12, %len23
  %bufsize5 = add i64 %totallen4, 1
  %concat.buf6 = alloca i8, i64 %bufsize5, align 1
  %2 = call ptr @memcpy(ptr %concat.buf6, ptr %concat.buf, i64 %len12)
  %dest27 = getelementptr i8, ptr %concat.buf6, i64 %len12
  %copylen28 = add i64 %len23, 1
  %3 = call ptr @memcpy(ptr %dest27, ptr @str.const.1, i64 %copylen28)
  %loadtmp = load ptr, ptr %concat.buf6, align 8
  ret ptr %loadtmp
}

define ptr @"\D9\88\D8\B3\D9\85_\D8\BA\D9\84\D9\82"(ptr %"\D8\A7\D8\B3\D9\85_\D9\88") {
entry:
  %"\D8\A7\D8\B3\D9\85_\D9\881" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85_\D9\88", ptr %"\D8\A7\D8\B3\D9\85_\D9\881", align 8
  %len1 = call i64 @strlen(ptr @str.const.2)
  %len2 = call i64 @strlen(ptr %"\D8\A7\D8\B3\D9\85_\D9\881")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.2, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\A7\D8\B3\D9\85_\D9\881", i64 %copylen2)
  %len12 = call i64 @strlen(ptr %concat.buf)
  %len23 = call i64 @strlen(ptr @str.const.1)
  %totallen4 = add i64 %len12, %len23
  %bufsize5 = add i64 %totallen4, 1
  %concat.buf6 = alloca i8, i64 %bufsize5, align 1
  %2 = call ptr @memcpy(ptr %concat.buf6, ptr %concat.buf, i64 %len12)
  %dest27 = getelementptr i8, ptr %concat.buf6, i64 %len12
  %copylen28 = add i64 %len23, 1
  %3 = call ptr @memcpy(ptr %dest27, ptr @str.const.1, i64 %copylen28)
  %loadtmp = load ptr, ptr %concat.buf6, align 8
  ret ptr %loadtmp
}

define ptr @"\D8\B9\D9\86\D8\B5\D8\B1_\D9\83\D8\A7\D9\85\D9\84"(ptr %"\D8\A7\D8\B3\D9\85_\D9\88", ptr %"\D9\85\D8\AD\D8\AA\D9\88\D9\89") {
entry:
  %"\D9\85\D8\AD\D8\AA\D9\88\D9\892" = alloca ptr, align 8
  store ptr %"\D9\85\D8\AD\D8\AA\D9\88\D9\89", ptr %"\D9\85\D8\AD\D8\AA\D9\88\D9\892", align 8
  %"\D8\A7\D8\B3\D9\85_\D9\881" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85_\D9\88", ptr %"\D8\A7\D8\B3\D9\85_\D9\881", align 8
  %calltmp = call ptr @"\D9\88\D8\B3\D9\85_\D9\81\D8\AA\D8\AD"(ptr %"\D8\A7\D8\B3\D9\85_\D9\881")
  %alloca = alloca ptr, align 8
  store ptr %calltmp, ptr %alloca, align 8
  %calltmp3 = call ptr @"\D9\88\D8\B3\D9\85_\D8\BA\D9\84\D9\82"(ptr %"\D8\A7\D8\B3\D9\85_\D9\881")
  %alloca4 = alloca ptr, align 8
  store ptr %calltmp3, ptr %alloca4, align 8
  %len1 = call i64 @strlen(ptr %alloca)
  %len2 = call i64 @strlen(ptr %"\D9\85\D8\AD\D8\AA\D9\88\D9\892")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr %alloca, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D9\85\D8\AD\D8\AA\D9\88\D9\892", i64 %copylen2)
  %len15 = call i64 @strlen(ptr %concat.buf)
  %len26 = call i64 @strlen(ptr %alloca4)
  %totallen7 = add i64 %len15, %len26
  %bufsize8 = add i64 %totallen7, 1
  %concat.buf9 = alloca i8, i64 %bufsize8, align 1
  %2 = call ptr @memcpy(ptr %concat.buf9, ptr %concat.buf, i64 %len15)
  %dest210 = getelementptr i8, ptr %concat.buf9, i64 %len15
  %copylen211 = add i64 %len26, 1
  %3 = call ptr @memcpy(ptr %dest210, ptr %alloca4, i64 %copylen211)
  %loadtmp = load ptr, ptr %concat.buf9, align 8
  ret ptr %loadtmp
}

define i64 @"\D8\B9\D8\B1\D8\B6_\D8\AD\D8\A7\D9\84\D8\A9"(ptr %"\D9\88\D8\B5\D9\81", i64 %"\D9\86\D8\A7\D8\AC\D8\AD") {
entry:
  %"\D9\86\D8\A7\D8\AC\D8\AD2" = alloca i64, align 8
  store i64 %"\D9\86\D8\A7\D8\AC\D8\AD", ptr %"\D9\86\D8\A7\D8\AC\D8\AD2", align 8
  %"\D9\88\D8\B5\D9\811" = alloca ptr, align 8
  store ptr %"\D9\88\D8\B5\D9\81", ptr %"\D9\88\D8\B5\D9\811", align 8
  %"%\D9\86\D8\A7\D8\AC\D8\AD.load" = load i64, ptr %"\D9\86\D8\A7\D8\AC\D8\AD2", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86\D8\A7\D8\AC\D8\AD.load", 1
  br i1 %cmpeqtmp, label %then_0, label %else_2

then_0:                                           ; preds = %entry
  %len1 = call i64 @strlen(ptr @str.const.4)
  %len2 = call i64 @strlen(ptr %"\D9\88\D8\B5\D9\811")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.4, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D9\88\D8\B5\D9\811", i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  br label %merge_1

else_2:                                           ; preds = %entry
  %len13 = call i64 @strlen(ptr @str.const.7)
  %len24 = call i64 @strlen(ptr %"\D9\88\D8\B5\D9\811")
  %totallen5 = add i64 %len13, %len24
  %bufsize6 = add i64 %totallen5, 1
  %concat.buf7 = alloca i8, i64 %bufsize6, align 1
  %4 = call ptr @memcpy(ptr %concat.buf7, ptr @str.const.7, i64 %len13)
  %dest28 = getelementptr i8, ptr %concat.buf7, i64 %len13
  %copylen29 = add i64 %len24, 1
  %5 = call ptr @memcpy(ptr %dest28, ptr %"\D9\88\D8\B5\D9\811", i64 %copylen29)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf7)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  br label %merge_1

merge_1:                                          ; preds = %else_2, %then_0
  ret i64 0
}

define ptr @"\D8\B7\D8\A8\D8\A7\D8\B9\D8\A9_\D8\B9\D9\86\D9\88\D8\A7\D9\86"(ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AF") {
entry:
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AF1" = alloca ptr, align 8
  store ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AF", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AF1", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  ret ptr @str.ret
}

define ptr @"\D8\B7\D8\A8\D8\A7\D8\B9\D8\A9_\D9\81\D9\82\D8\B1\D8\A9"(ptr %"\D9\88\D8\B5\D9\81_\D8\AF") {
entry:
  %"\D9\88\D8\B5\D9\81_\D8\AF1" = alloca ptr, align 8
  store ptr %"\D9\88\D8\B5\D9\81_\D8\AF", ptr %"\D9\88\D8\B5\D9\81_\D8\AF1", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  ret ptr @str.ret.17
}

define ptr @"\D8\B7\D8\A8\D8\A7\D8\B9\D8\A9_\D9\85\D8\B9\D8\A7\D9\85\D9\84"(ptr %"\D8\A7\D8\B3\D9\85_\D9\85", ptr %"\D9\86\D9\88\D8\B9_\D9\85", ptr %"\D9\88\D8\B5\D9\81_\D9\85") {
entry:
  %"\D9\88\D8\B5\D9\81_\D9\853" = alloca ptr, align 8
  store ptr %"\D9\88\D8\B5\D9\81_\D9\85", ptr %"\D9\88\D8\B5\D9\81_\D9\853", align 8
  %"\D9\86\D9\88\D8\B9_\D9\852" = alloca ptr, align 8
  store ptr %"\D9\86\D9\88\D8\B9_\D9\85", ptr %"\D9\86\D9\88\D8\B9_\D9\852", align 8
  %"\D8\A7\D8\B3\D9\85_\D9\851" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85_\D9\85", ptr %"\D8\A7\D8\B3\D9\85_\D9\851", align 8
  %calltmp = call ptr @"\D8\B9\D9\86\D8\B5\D8\B1_\D9\83\D8\A7\D9\85\D9\84"(ptr @str.const.18, ptr %"\D9\86\D9\88\D8\B9_\D9\852")
  %alloca = alloca ptr, align 8
  store ptr %calltmp, ptr %alloca, align 8
  %alloca4 = alloca ptr, align 8
  %len1 = call i64 @strlen(ptr %alloca4)
  %len2 = call i64 @strlen(ptr %alloca)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr %alloca4, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %alloca, i64 %copylen2)
  %alloca5 = alloca ptr, align 8
  store ptr %concat.buf, ptr %alloca5, align 8
  %len16 = call i64 @strlen(ptr %alloca5)
  %len27 = call i64 @strlen(ptr @str.const.20)
  %totallen8 = add i64 %len16, %len27
  %bufsize9 = add i64 %totallen8, 1
  %concat.buf10 = alloca i8, i64 %bufsize9, align 1
  %2 = call ptr @memcpy(ptr %concat.buf10, ptr %alloca5, i64 %len16)
  %dest211 = getelementptr i8, ptr %concat.buf10, i64 %len16
  %copylen212 = add i64 %len27, 1
  %3 = call ptr @memcpy(ptr %dest211, ptr @str.const.20, i64 %copylen212)
  %alloca13 = alloca ptr, align 8
  store ptr %concat.buf10, ptr %alloca13, align 8
  %len114 = call i64 @strlen(ptr %alloca13)
  %len215 = call i64 @strlen(ptr %"\D9\88\D8\B5\D9\81_\D9\853")
  %totallen16 = add i64 %len114, %len215
  %bufsize17 = add i64 %totallen16, 1
  %concat.buf18 = alloca i8, i64 %bufsize17, align 1
  %4 = call ptr @memcpy(ptr %concat.buf18, ptr %alloca13, i64 %len114)
  %dest219 = getelementptr i8, ptr %concat.buf18, i64 %len114
  %copylen220 = add i64 %len215, 1
  %5 = call ptr @memcpy(ptr %dest219, ptr %"\D9\88\D8\B5\D9\81_\D9\853", i64 %copylen220)
  %alloca21 = alloca ptr, align 8
  store ptr %concat.buf18, ptr %alloca21, align 8
  %calltmp22 = call ptr @"\D8\B9\D9\86\D8\B5\D8\B1_\D9\83\D8\A7\D9\85\D9\84"(ptr @str.const.21, ptr %alloca21)
  %alloca23 = alloca ptr, align 8
  store ptr %calltmp22, ptr %alloca23, align 8
  %len124 = call i64 @strlen(ptr @str.const.22)
  %len225 = call i64 @strlen(ptr %alloca23)
  %totallen26 = add i64 %len124, %len225
  %bufsize27 = add i64 %totallen26, 1
  %concat.buf28 = alloca i8, i64 %bufsize27, align 1
  %6 = call ptr @memcpy(ptr %concat.buf28, ptr @str.const.22, i64 %len124)
  %dest229 = getelementptr i8, ptr %concat.buf28, i64 %len124
  %copylen230 = add i64 %len225, 1
  %7 = call ptr @memcpy(ptr %dest229, ptr %alloca23, i64 %copylen230)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf28)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  ret ptr @str.ret.26
}

define ptr @"\D8\B7\D8\A8\D8\A7\D8\B9\D8\A9_\D8\A7\D8\B1\D8\AC\D8\A7\D8\B9"(ptr %"\D9\86\D9\88\D8\B9_\D8\B1", ptr %"\D9\88\D8\B5\D9\81_\D8\B1") {
entry:
  %"\D9\88\D8\B5\D9\81_\D8\B12" = alloca ptr, align 8
  store ptr %"\D9\88\D8\B5\D9\81_\D8\B1", ptr %"\D9\88\D8\B5\D9\81_\D8\B12", align 8
  %"\D9\86\D9\88\D8\B9_\D8\B11" = alloca ptr, align 8
  store ptr %"\D9\86\D9\88\D8\B9_\D8\B1", ptr %"\D9\86\D9\88\D8\B9_\D8\B11", align 8
  %alloca = alloca ptr, align 8
  %len1 = call i64 @strlen(ptr %alloca)
  %len2 = call i64 @strlen(ptr @str.const.28)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr %alloca, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr @str.const.28, i64 %copylen2)
  %len13 = call i64 @strlen(ptr %concat.buf)
  %len24 = call i64 @strlen(ptr %"\D9\88\D8\B5\D9\81_\D8\B12")
  %totallen5 = add i64 %len13, %len24
  %bufsize6 = add i64 %totallen5, 1
  %concat.buf7 = alloca i8, i64 %bufsize6, align 1
  %2 = call ptr @memcpy(ptr %concat.buf7, ptr %concat.buf, i64 %len13)
  %dest28 = getelementptr i8, ptr %concat.buf7, i64 %len13
  %copylen29 = add i64 %len24, 1
  %3 = call ptr @memcpy(ptr %dest28, ptr %"\D9\88\D8\B5\D9\81_\D8\B12", i64 %copylen29)
  %alloca10 = alloca ptr, align 8
  store ptr %concat.buf7, ptr %alloca10, align 8
  %calltmp = call ptr @"\D8\B9\D9\86\D8\B5\D8\B1_\D9\83\D8\A7\D9\85\D9\84"(ptr @str.const.29, ptr %alloca10)
  %alloca11 = alloca ptr, align 8
  store ptr %calltmp, ptr %alloca11, align 8
  %len112 = call i64 @strlen(ptr @str.const.11)
  %len213 = call i64 @strlen(ptr %alloca11)
  %totallen14 = add i64 %len112, %len213
  %bufsize15 = add i64 %totallen14, 1
  %concat.buf16 = alloca i8, i64 %bufsize15, align 1
  %4 = call ptr @memcpy(ptr %concat.buf16, ptr @str.const.11, i64 %len112)
  %dest217 = getelementptr i8, ptr %concat.buf16, i64 %len112
  %copylen218 = add i64 %len213, 1
  %5 = call ptr @memcpy(ptr %dest217, ptr %alloca11, i64 %copylen218)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf16)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  ret ptr @str.ret.34
}

define i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D9\88\D8\AB\D9\8A\D9\82\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.35)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.39)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.43)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.47)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %calltmp = call ptr @"\D8\B7\D8\A8\D8\A7\D8\B9\D8\A9_\D8\B9\D9\86\D9\88\D8\A7\D9\86"(ptr @str.const.51)
  %calltmp1 = call ptr @"\D8\B7\D8\A8\D8\A7\D8\B9\D8\A9_\D9\81\D9\82\D8\B1\D8\A9"(ptr @str.const.52)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.53)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %calltmp2 = call ptr @"\D8\B7\D8\A8\D8\A7\D8\B9\D8\A9_\D9\85\D8\B9\D8\A7\D9\85\D9\84"(ptr @str.const.57, ptr @str.const.58, ptr @str.const.59)
  %calltmp3 = call ptr @"\D8\B7\D8\A8\D8\A7\D8\B9\D8\A9_\D9\85\D8\B9\D8\A7\D9\85\D9\84"(ptr @str.const.60, ptr @str.const.58, ptr @str.const.62)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.63)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %calltmp4 = call ptr @"\D8\B7\D8\A8\D8\A7\D8\B9\D8\A9_\D8\A7\D8\B1\D8\AC\D8\A7\D8\B9"(ptr @str.const.58, ptr @str.const.68)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.69)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %calltmp5 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AD\D8\A7\D9\84\D8\A9"(ptr @str.const.73, i64 1)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.74)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  ret i64 0
}

define i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D9\81\D9\87\D8\B1\D8\B3"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.78)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.82)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.86)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.90)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %calltmp = call ptr @"\D8\B9\D9\86\D8\B5\D8\B1_\D9\83\D8\A7\D9\85\D9\84"(ptr @str.const.94, ptr @str.const.95)
  %alloca = alloca ptr, align 8
  store ptr %calltmp, ptr %alloca, align 8
  %len1 = call i64 @strlen(ptr @str.const.96)
  %len2 = call i64 @strlen(ptr %alloca)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %8 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.96, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %9 = call ptr @memcpy(ptr %dest2, ptr %alloca, i64 %copylen2)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.100)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.53)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %calltmp1 = call ptr @"\D8\B9\D9\86\D8\B5\D8\B1_\D9\83\D8\A7\D9\85\D9\84"(ptr @str.const.108, ptr @str.const.109)
  %alloca2 = alloca ptr, align 8
  store ptr %calltmp1, ptr %alloca2, align 8
  %calltmp3 = call ptr @"\D8\B9\D9\86\D8\B5\D8\B1_\D9\83\D8\A7\D9\85\D9\84"(ptr @str.const.108, ptr @str.const.111)
  %alloca4 = alloca ptr, align 8
  store ptr %calltmp3, ptr %alloca4, align 8
  %calltmp5 = call ptr @"\D8\B9\D9\86\D8\B5\D8\B1_\D9\83\D8\A7\D9\85\D9\84"(ptr @str.const.108, ptr @str.const.113)
  %alloca6 = alloca ptr, align 8
  store ptr %calltmp5, ptr %alloca6, align 8
  %calltmp7 = call ptr @"\D8\B9\D9\86\D8\B5\D8\B1_\D9\83\D8\A7\D9\85\D9\84"(ptr @str.const.108, ptr @str.const.115)
  %alloca8 = alloca ptr, align 8
  store ptr %calltmp7, ptr %alloca8, align 8
  %calltmp9 = call ptr @"\D8\B9\D9\86\D8\B5\D8\B1_\D9\83\D8\A7\D9\85\D9\84"(ptr @str.const.21, ptr %alloca2)
  %alloca10 = alloca ptr, align 8
  store ptr %calltmp9, ptr %alloca10, align 8
  %calltmp11 = call ptr @"\D8\B9\D9\86\D8\B5\D8\B1_\D9\83\D8\A7\D9\85\D9\84"(ptr @str.const.21, ptr %alloca4)
  %alloca12 = alloca ptr, align 8
  store ptr %calltmp11, ptr %alloca12, align 8
  %calltmp13 = call ptr @"\D8\B9\D9\86\D8\B5\D8\B1_\D9\83\D8\A7\D9\85\D9\84"(ptr @str.const.21, ptr %alloca6)
  %alloca14 = alloca ptr, align 8
  store ptr %calltmp13, ptr %alloca14, align 8
  %calltmp15 = call ptr @"\D8\B9\D9\86\D8\B5\D8\B1_\D9\83\D8\A7\D9\85\D9\84"(ptr @str.const.21, ptr %alloca8)
  %alloca16 = alloca ptr, align 8
  store ptr %calltmp15, ptr %alloca16, align 8
  %len117 = call i64 @strlen(ptr @str.const.22)
  %len218 = call i64 @strlen(ptr %alloca10)
  %totallen19 = add i64 %len117, %len218
  %bufsize20 = add i64 %totallen19, 1
  %concat.buf21 = alloca i8, i64 %bufsize20, align 1
  %16 = call ptr @memcpy(ptr %concat.buf21, ptr @str.const.22, i64 %len117)
  %dest222 = getelementptr i8, ptr %concat.buf21, i64 %len117
  %copylen223 = add i64 %len218, 1
  %17 = call ptr @memcpy(ptr %dest222, ptr %alloca10, i64 %copylen223)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf21)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %len124 = call i64 @strlen(ptr @str.const.22)
  %len225 = call i64 @strlen(ptr %alloca12)
  %totallen26 = add i64 %len124, %len225
  %bufsize27 = add i64 %totallen26, 1
  %concat.buf28 = alloca i8, i64 %bufsize27, align 1
  %20 = call ptr @memcpy(ptr %concat.buf28, ptr @str.const.22, i64 %len124)
  %dest229 = getelementptr i8, ptr %concat.buf28, i64 %len124
  %copylen230 = add i64 %len225, 1
  %21 = call ptr @memcpy(ptr %dest229, ptr %alloca12, i64 %copylen230)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf28)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %len131 = call i64 @strlen(ptr @str.const.22)
  %len232 = call i64 @strlen(ptr %alloca14)
  %totallen33 = add i64 %len131, %len232
  %bufsize34 = add i64 %totallen33, 1
  %concat.buf35 = alloca i8, i64 %bufsize34, align 1
  %24 = call ptr @memcpy(ptr %concat.buf35, ptr @str.const.22, i64 %len131)
  %dest236 = getelementptr i8, ptr %concat.buf35, i64 %len131
  %copylen237 = add i64 %len232, 1
  %25 = call ptr @memcpy(ptr %dest236, ptr %alloca14, i64 %copylen237)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf35)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %len138 = call i64 @strlen(ptr @str.const.22)
  %len239 = call i64 @strlen(ptr %alloca16)
  %totallen40 = add i64 %len138, %len239
  %bufsize41 = add i64 %totallen40, 1
  %concat.buf42 = alloca i8, i64 %bufsize41, align 1
  %28 = call ptr @memcpy(ptr %concat.buf42, ptr @str.const.22, i64 %len138)
  %dest243 = getelementptr i8, ptr %concat.buf42, i64 %len138
  %copylen244 = add i64 %len239, 1
  %29 = call ptr @memcpy(ptr %dest243, ptr %alloca16, i64 %copylen244)
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf42)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.63)
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %34 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.140)
  %35 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %36 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.144)
  %37 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %alloca45 = alloca i64, align 8
  store i64 4, ptr %alloca45, align 8
  %alloca46 = alloca ptr, align 8
  %len147 = call i64 @strlen(ptr @str.const.148)
  %len248 = call i64 @strlen(ptr %alloca46)
  %totallen49 = add i64 %len147, %len248
  %bufsize50 = add i64 %totallen49, 1
  %concat.buf51 = alloca i8, i64 %bufsize50, align 1
  %38 = call ptr @memcpy(ptr %concat.buf51, ptr @str.const.148, i64 %len147)
  %dest252 = getelementptr i8, ptr %concat.buf51, i64 %len147
  %copylen253 = add i64 %len248, 1
  %39 = call ptr @memcpy(ptr %dest252, ptr %alloca46, i64 %copylen253)
  %40 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf51)
  %41 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %calltmp54 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AD\D8\A7\D9\84\D8\A9"(ptr @str.const.152, i64 1)
  %42 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.153)
  %43 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  ret i64 0
}

define i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D9\88\D8\B3\D9\88\D9\85"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.157)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.161)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %calltmp = call ptr @"\D8\B9\D9\86\D8\B5\D8\B1_\D9\83\D8\A7\D9\85\D9\84"(ptr @str.const.165, ptr @str.const.166)
  %alloca1 = alloca ptr, align 8
  store ptr %calltmp, ptr %alloca1, align 8
  %strlen.ret = call i64 @strlen(ptr %alloca1)
  %alloca2 = alloca i64, align 8
  store i64 %strlen.ret, ptr %alloca2, align 8
  %"%\D8\B71.load" = load i64, ptr %alloca2, align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B71.load", 10
  br i1 %cmpgttmp, label %then_3, label %else_5

then_3:                                           ; preds = %entry
  %calltmp3 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AD\D8\A7\D9\84\D8\A9"(ptr @str.const.167, i64 1)
  %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load", 1
  store i64 %addtmp, ptr %alloca, align 8
  br label %merge_4

else_5:                                           ; preds = %entry
  %calltmp4 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AD\D8\A7\D9\84\D8\A9"(ptr @str.const.167, i64 0)
  br label %merge_4

merge_4:                                          ; preds = %else_5, %then_3
  %calltmp5 = call ptr @"\D9\88\D8\B3\D9\85_\D9\81\D8\AA\D8\AD"(ptr @str.const.169)
  %alloca6 = alloca ptr, align 8
  store ptr %calltmp5, ptr %alloca6, align 8
  %len1 = call i64 @strlen(ptr @str.const.170)
  %len2 = call i64 @strlen(ptr %alloca6)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %4 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.170, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %5 = call ptr @memcpy(ptr %dest2, ptr %alloca6, i64 %copylen2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load7" = load i64, ptr %alloca, align 8
  %addtmp8 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load7", 1
  store i64 %addtmp8, ptr %alloca, align 8
  %calltmp9 = call ptr @"\D9\88\D8\B3\D9\85_\D8\BA\D9\84\D9\82"(ptr @str.const.169)
  %alloca10 = alloca ptr, align 8
  store ptr %calltmp9, ptr %alloca10, align 8
  %len111 = call i64 @strlen(ptr @str.const.175)
  %len212 = call i64 @strlen(ptr %alloca10)
  %totallen13 = add i64 %len111, %len212
  %bufsize14 = add i64 %totallen13, 1
  %concat.buf15 = alloca i8, i64 %bufsize14, align 1
  %8 = call ptr @memcpy(ptr %concat.buf15, ptr @str.const.175, i64 %len111)
  %dest216 = getelementptr i8, ptr %concat.buf15, i64 %len111
  %copylen217 = add i64 %len212, 1
  %9 = call ptr @memcpy(ptr %dest216, ptr %alloca10, i64 %copylen217)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf15)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load18" = load i64, ptr %alloca, align 8
  %addtmp19 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load18", 1
  store i64 %addtmp19, ptr %alloca, align 8
  %calltmp20 = call ptr @"\D8\B9\D9\86\D8\B5\D8\B1_\D9\83\D8\A7\D9\85\D9\84"(ptr @str.const.179, ptr @str.const.180)
  %alloca21 = alloca ptr, align 8
  store ptr %calltmp20, ptr %alloca21, align 8
  %calltmp22 = call ptr @"\D8\B9\D9\86\D8\B5\D8\B1_\D9\83\D8\A7\D9\85\D9\84"(ptr @str.const.29, ptr %alloca21)
  %alloca23 = alloca ptr, align 8
  store ptr %calltmp22, ptr %alloca23, align 8
  %len124 = call i64 @strlen(ptr @str.const.182)
  %len225 = call i64 @strlen(ptr %alloca23)
  %totallen26 = add i64 %len124, %len225
  %bufsize27 = add i64 %totallen26, 1
  %concat.buf28 = alloca i8, i64 %bufsize27, align 1
  %12 = call ptr @memcpy(ptr %concat.buf28, ptr @str.const.182, i64 %len124)
  %dest229 = getelementptr i8, ptr %concat.buf28, i64 %len124
  %copylen230 = add i64 %len225, 1
  %13 = call ptr @memcpy(ptr %dest229, ptr %alloca23, i64 %copylen230)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf28)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load31" = load i64, ptr %alloca, align 8
  %addtmp32 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load31", 1
  store i64 %addtmp32, ptr %alloca, align 8
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.186)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %alloca33 = alloca ptr, align 8
  %len134 = call i64 @strlen(ptr @str.const.190)
  %len235 = call i64 @strlen(ptr %alloca33)
  %totallen36 = add i64 %len134, %len235
  %bufsize37 = add i64 %totallen36, 1
  %concat.buf38 = alloca i8, i64 %bufsize37, align 1
  %18 = call ptr @memcpy(ptr %concat.buf38, ptr @str.const.190, i64 %len134)
  %dest239 = getelementptr i8, ptr %concat.buf38, i64 %len134
  %copylen240 = add i64 %len235, 1
  %19 = call ptr @memcpy(ptr %dest239, ptr %alloca33, i64 %copylen240)
  %len141 = call i64 @strlen(ptr %concat.buf38)
  %len242 = call i64 @strlen(ptr @str.const.191)
  %totallen43 = add i64 %len141, %len242
  %bufsize44 = add i64 %totallen43, 1
  %concat.buf45 = alloca i8, i64 %bufsize44, align 1
  %20 = call ptr @memcpy(ptr %concat.buf45, ptr %concat.buf38, i64 %len141)
  %dest246 = getelementptr i8, ptr %concat.buf45, i64 %len141
  %copylen247 = add i64 %len242, 1
  %21 = call ptr @memcpy(ptr %dest246, ptr @str.const.191, i64 %copylen247)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf45)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.195)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  ret i64 0
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.199)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.203)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.207)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.211)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %calltmp = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D9\88\D8\AB\D9\8A\D9\82\D8\A9"()
  %calltmp1 = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D9\81\D9\87\D8\B1\D8\B3"()
  %calltmp2 = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D9\88\D8\B3\D9\88\D9\85"()
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.215)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
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
