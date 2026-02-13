; ModuleID = '06_complex_database'
source_filename = "06_complex_database"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.ret = private unnamed_addr constant [31 x i8] c"==============================\00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.2 = private unnamed_addr constant [3 x i8] c"  \00", align 1
@str.const.9 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.10 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.11 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.12 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.13 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.14 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.15 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.16 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.17 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.18 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.19 = private unnamed_addr constant [24 x i8] c"+ \D8\A5\D8\B6\D8\A7\D9\81\D8\A9 \D9\85\D9\88\D8\B8\D9\81: \00", align 1
@str.const.20 = private unnamed_addr constant [4 x i8] c" | \00", align 1
@str.const.24 = private unnamed_addr constant [56 x i8] c"| \D8\A7\D9\84\D8\A7\D8\B3\D9\85         | \D8\A7\D9\84\D9\82\D8\B3\D9\85      | \D8\A7\D9\84\D8\B1\D8\A7\D8\AA\D8\A8 |\00", align 1
@str.const.28 = private unnamed_addr constant [40 x i8] c"|---------------|------------|--------|\00", align 1
@str.const.32 = private unnamed_addr constant [3 x i8] c"| \00", align 1
@str.const.35 = private unnamed_addr constant [3 x i8] c" |\00", align 1
@str.const.62 = private unnamed_addr constant [17 x i8] c"\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9: \00", align 1
@str.const.63 = private unnamed_addr constant [10 x i8] c" \D9\85\D9\88\D8\B8\D9\81\00", align 1
@str.const.66 = private unnamed_addr constant [33 x i8] c"\D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA \D8\A7\D9\84\D8\B1\D9\88\D8\A7\D8\AA\D8\A8:\00", align 1
@str.const.70 = private unnamed_addr constant [19 x i8] c"  \D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9: \00", align 1
@str.const.73 = private unnamed_addr constant [19 x i8] c"  \D8\A7\D9\84\D9\85\D8\AA\D9\88\D8\B3\D8\B7: \00", align 1
@str.const.76 = private unnamed_addr constant [17 x i8] c"  \D8\A7\D9\84\D8\A3\D8\B9\D9\84\D9\89: \00", align 1
@str.const.79 = private unnamed_addr constant [17 x i8] c"  \D8\A7\D9\84\D8\A3\D8\AF\D9\86\D9\89: \00", align 1
@str.const.82 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.83 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.84 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.85 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.86 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.87 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.88 = private unnamed_addr constant [15 x i8] c"+ \D9\85\D8\B4\D8\B1\D9\88\D8\B9: \00", align 1
@str.const.92 = private unnamed_addr constant [2 x i8] c"%\00", align 1
@str.const.95 = private unnamed_addr constant [79 x i8] c"| \D8\A7\D9\84\D9\85\D8\B4\D8\B1\D9\88\D8\B9      | \D8\A7\D9\84\D8\AD\D8\A7\D9\84\D8\A9     | \D8\A7\D9\84\D9\85\D9\8A\D8\B2\D8\A7\D9\86\D9\8A\D8\A9 | \D8\A7\D9\84\D8\AA\D9\82\D8\AF\D9\85 |\00", align 1
@str.const.99 = private unnamed_addr constant [49 x i8] c"|--------------|-----------|----------|--------|\00", align 1
@str.const.107 = private unnamed_addr constant [4 x i8] c"% |\00", align 1
@str.const.124 = private unnamed_addr constant [30 x i8] c"\D8\A7\D9\84\D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \D8\A7\D9\84\D9\86\D9\87\D8\A7\D8\A6\D9\8A\00", align 1
@str.const.125 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.129 = private unnamed_addr constant [25 x i8] c"--- \D8\A7\D9\84\D9\85\D9\88\D8\B8\D9\81\D9\88\D9\86 ---\00", align 1
@str.const.133 = private unnamed_addr constant [30 x i8] c"  \D9\85\D8\AC\D9\85\D9\88\D8\B9 \D8\A7\D9\84\D8\B1\D9\88\D8\A7\D8\AA\D8\A8: \00", align 1
@str.const.136 = private unnamed_addr constant [30 x i8] c"  \D9\85\D8\AA\D9\88\D8\B3\D8\B7 \D8\A7\D9\84\D8\B1\D9\88\D8\A7\D8\AA\D8\A8: \00", align 1
@str.const.139 = private unnamed_addr constant [22 x i8] c"  \D8\A3\D8\B9\D9\84\D9\89 \D8\B1\D8\A7\D8\AA\D8\A8: \00", align 1
@str.const.142 = private unnamed_addr constant [22 x i8] c"  \D8\A3\D8\AF\D9\86\D9\89 \D8\B1\D8\A7\D8\AA\D8\A8: \00", align 1
@str.const.145 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.149 = private unnamed_addr constant [25 x i8] c"--- \D8\A7\D9\84\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9 ---\00", align 1
@str.const.153 = private unnamed_addr constant [28 x i8] c"  \D8\B9\D8\AF\D8\AF \D8\A7\D9\84\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9: \00", align 1
@str.const.156 = private unnamed_addr constant [36 x i8] c"  \D9\85\D8\AC\D9\85\D9\88\D8\B9 \D8\A7\D9\84\D9\85\D9\8A\D8\B2\D8\A7\D9\86\D9\8A\D8\A7\D8\AA: \00", align 1
@str.const.159 = private unnamed_addr constant [28 x i8] c"  \D9\85\D8\AA\D9\88\D8\B3\D8\B7 \D8\A7\D9\84\D8\AA\D9\82\D8\AF\D9\85: \00", align 1
@str.const.163 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.167 = private unnamed_addr constant [34 x i8] c"--- \D9\85\D8\A4\D8\B4\D8\B1\D8\A7\D8\AA \D8\A7\D9\84\D8\A3\D8\AF\D8\A7\D8\A1 ---\00", align 1
@str.const.171 = private unnamed_addr constant [27 x i8] c"  \D8\A7\D9\84\D8\A3\D8\AF\D8\A7\D8\A1: \D9\85\D9\85\D8\AA\D8\A7\D8\B2\00", align 1
@str.const.175 = private unnamed_addr constant [23 x i8] c"  \D8\A7\D9\84\D8\A3\D8\AF\D8\A7\D8\A1: \D8\AC\D9\8A\D8\AF\00", align 1
@str.const.179 = private unnamed_addr constant [38 x i8] c"  \D8\A7\D9\84\D8\A3\D8\AF\D8\A7\D8\A1: \D9\8A\D8\AD\D8\AA\D8\A7\D8\AC \D8\AA\D8\AD\D8\B3\D9\8A\D9\86\00", align 1
@str.const.183 = private unnamed_addr constant [45 x i8] c"  \D8\AA\D8\AD\D8\B0\D9\8A\D8\B1: \D9\81\D8\AC\D9\88\D8\A9 \D8\B1\D9\88\D8\A7\D8\AA\D8\A8 \D9\83\D8\A8\D9\8A\D8\B1\D8\A9\00", align 1
@str.const.187 = private unnamed_addr constant [33 x i8] c"  \D8\A7\D9\84\D8\B1\D9\88\D8\A7\D8\AA\D8\A8: \D9\85\D8\AA\D9\88\D8\A7\D8\B2\D9\86\D8\A9\00", align 1
@str.const.191 = private unnamed_addr constant [46 x i8] c"\D9\86\D8\B8\D8\A7\D9\85 \D9\82\D8\A7\D8\B9\D8\AF\D8\A9 \D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA \D8\A7\D9\84\D8\B4\D8\B1\D9\83\D8\A9\00", align 1
@str.const.192 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.196 = private unnamed_addr constant [52 x i8] c"=== 1. \D8\A5\D8\AF\D8\AE\D8\A7\D9\84 \D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA \D8\A7\D9\84\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86 ===\00", align 1
@str.const.200 = private unnamed_addr constant [9 x i8] c"\D8\A3\D8\AD\D9\85\D8\AF\00", align 1
@str.const.201 = private unnamed_addr constant [11 x i8] c"\D9\87\D9\86\D8\AF\D8\B3\D8\A9\00", align 1
@str.const.202 = private unnamed_addr constant [9 x i8] c"\D8\B3\D8\A7\D8\B1\D8\A9\00", align 1
@str.const.203 = private unnamed_addr constant [11 x i8] c"\D8\A5\D8\AF\D8\A7\D8\B1\D8\A9\00", align 1
@str.const.204 = private unnamed_addr constant [9 x i8] c"\D8\AE\D8\A7\D9\84\D8\AF\00", align 1
@str.const.205 = private unnamed_addr constant [11 x i8] c"\D8\AA\D8\B3\D9\88\D9\8A\D9\82\00", align 1
@str.const.206 = private unnamed_addr constant [11 x i8] c"\D9\81\D8\A7\D8\B7\D9\85\D8\A9\00", align 1
@str.const.208 = private unnamed_addr constant [7 x i8] c"\D8\B9\D9\85\D8\B1\00", align 1
@str.const.209 = private unnamed_addr constant [7 x i8] c"\D8\AF\D8\B9\D9\85\00", align 1
@str.const.210 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.214 = private unnamed_addr constant [37 x i8] c"=== 2. \D8\AC\D8\AF\D9\88\D9\84 \D8\A7\D9\84\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86 ===\00", align 1
@str.const.218 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.222 = private unnamed_addr constant [43 x i8] c"=== 3. \D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA \D8\A7\D9\84\D8\B1\D9\88\D8\A7\D8\AA\D8\A8 ===\00", align 1
@str.const.226 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.230 = private unnamed_addr constant [52 x i8] c"=== 4. \D8\A5\D8\AF\D8\AE\D8\A7\D9\84 \D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA \D8\A7\D9\84\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9 ===\00", align 1
@str.const.234 = private unnamed_addr constant [16 x i8] c"\D9\85\D9\86\D8\B5\D8\A9_\D9\88\D9\8A\D8\A8\00", align 1
@str.const.235 = private unnamed_addr constant [7 x i8] c"\D9\86\D8\B4\D8\B7\00", align 1
@str.const.236 = private unnamed_addr constant [20 x i8] c"\D8\AA\D8\B7\D8\A8\D9\8A\D9\82_\D8\AC\D9\88\D8\A7\D9\84\00", align 1
@str.const.237 = private unnamed_addr constant [11 x i8] c"\D8\AA\D8\B7\D9\88\D9\8A\D8\B1\00", align 1
@str.const.238 = private unnamed_addr constant [15 x i8] c"\D8\AA\D8\AD\D9\84\D9\8A\D9\84\D8\A7\D8\AA\00", align 1
@str.const.239 = private unnamed_addr constant [11 x i8] c"\D8\AA\D8\AE\D8\B7\D9\8A\D8\B7\00", align 1
@str.const.240 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.244 = private unnamed_addr constant [37 x i8] c"=== 5. \D8\AC\D8\AF\D9\88\D9\84 \D8\A7\D9\84\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9 ===\00", align 1
@str.const.248 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.252 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.256 = private unnamed_addr constant [35 x i8] c"\D8\A7\D9\86\D8\AA\D9\87\D9\89 \D8\A7\D9\84\D9\86\D8\B8\D8\A7\D9\85 \D8\A8\D9\86\D8\AC\D8\A7\D8\AD\00", align 1

define ptr @"\D9\81\D8\A7\D8\B5\D9\84_\D8\B7\D9\88\D9\8A\D9\84"() {
entry:
  ret ptr @str.ret
}

define i64 @"\D8\A7\D8\B9\D8\B1\D8\B6_\D8\B9\D9\86\D9\88\D8\A7\D9\86"(ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86") {
entry:
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\861" = alloca ptr, align 8
  store ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  %calltmp = call ptr @"\D9\81\D8\A7\D8\B5\D9\84_\D8\B7\D9\88\D9\8A\D9\84"()
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %calltmp)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %len1 = call i64 @strlen(ptr @str.const.2)
  %len2 = call i64 @strlen(ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %2 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.2, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %3 = call ptr @memcpy(ptr %dest2, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861", i64 %copylen2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %calltmp2 = call ptr @"\D9\81\D8\A7\D8\B5\D9\84_\D8\B7\D9\88\D9\8A\D9\84"()
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %calltmp2)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  ret i64 0
}

define void @"\D8\AC\D8\AF\D9\88\D9\84_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca ptr, align 8
  %alloca4 = alloca ptr, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca ptr, align 8
  %alloca7 = alloca ptr, align 8
  %alloca8 = alloca i64, align 8
  %alloca9 = alloca i64, align 8
  %alloca10 = alloca i64, align 8
  %alloca11 = alloca i64, align 8
  %alloca12 = alloca ptr, align 8
  %alloca13 = alloca ptr, align 8
  %alloca14 = alloca ptr, align 8
  %alloca15 = alloca ptr, align 8
  %alloca16 = alloca ptr, align 8
  store i64 0, ptr %alloca, align 8
  store ptr @str.const.9, ptr %alloca2, align 8
  store ptr @str.const.10, ptr %alloca3, align 8
  store ptr @str.const.11, ptr %alloca4, align 8
  store ptr @str.const.12, ptr %alloca6, align 8
  store ptr @str.const.13, ptr %alloca7, align 8
  store i64 0, ptr %alloca8, align 8
  store i64 0, ptr %alloca5, align 8
  store i64 0, ptr %alloca9, align 8
  store i64 0, ptr %alloca10, align 8
  store i64 0, ptr %alloca11, align 8
  store ptr @str.const.14, ptr %alloca12, align 8
  store ptr @str.const.15, ptr %alloca13, align 8
  store ptr @str.const.16, ptr %alloca14, align 8
  store ptr @str.const.17, ptr %alloca15, align 8
  store ptr @str.const.18, ptr %alloca16, align 8
  ret void
}

define void @"\D8\AC\D8\AF\D9\88\D9\84_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.\D8\A3\D8\B6\D9\81"(i64 %self, ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85", i64 %"\D8\A7\D9\84\D8\B1\D8\A7\D8\AA\D8\A8", ptr %"\D8\A7\D9\84\D9\82\D8\B3\D9\85") {
entry:
  %"\D8\A7\D9\84\D9\82\D8\B3\D9\854" = alloca ptr, align 8
  store ptr %"\D8\A7\D9\84\D9\82\D8\B3\D9\85", ptr %"\D8\A7\D9\84\D9\82\D8\B3\D9\854", align 8
  %"\D8\A7\D9\84\D8\B1\D8\A7\D8\AA\D8\A83" = alloca i64, align 8
  store i64 %"\D8\A7\D9\84\D8\B1\D8\A7\D8\AA\D8\A8", ptr %"\D8\A7\D9\84\D8\B1\D8\A7\D8\AA\D8\A83", align 8
  %"\D8\A7\D9\84\D8\A7\D8\B3\D9\852" = alloca ptr, align 8
  store ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85", ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\852", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca5 = alloca ptr, align 8
  %alloca6 = alloca ptr, align 8
  %alloca7 = alloca ptr, align 8
  %alloca8 = alloca i64, align 8
  %alloca9 = alloca ptr, align 8
  %alloca10 = alloca ptr, align 8
  %alloca11 = alloca i64, align 8
  %alloca12 = alloca i64, align 8
  %alloca13 = alloca i64, align 8
  %alloca14 = alloca i64, align 8
  %alloca15 = alloca ptr, align 8
  %alloca16 = alloca ptr, align 8
  %alloca17 = alloca ptr, align 8
  %alloca18 = alloca ptr, align 8
  %alloca19 = alloca ptr, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load", 1
  store i64 %addtmp, ptr %alloca, align 8
  %loadtmp = load i64, ptr %alloca, align 8
  br label %match.case0.test_1

match.end_0:                                      ; preds = %match.case4.body_10, %match.case4.test_9, %match.case3.body_8, %match.case2.body_6, %match.case1.body_4, %match.case0.body_2
  %len1 = call i64 @strlen(ptr @str.const.19)
  %len2 = call i64 @strlen(ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\852")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.19, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\852", i64 %copylen2)
  %len120 = call i64 @strlen(ptr %concat.buf)
  %len221 = call i64 @strlen(ptr @str.const.20)
  %totallen22 = add i64 %len120, %len221
  %bufsize23 = add i64 %totallen22, 1
  %concat.buf24 = alloca i8, i64 %bufsize23, align 1
  %2 = call ptr @memcpy(ptr %concat.buf24, ptr %concat.buf, i64 %len120)
  %dest225 = getelementptr i8, ptr %concat.buf24, i64 %len120
  %copylen226 = add i64 %len221, 1
  %3 = call ptr @memcpy(ptr %dest225, ptr @str.const.20, i64 %copylen226)
  %len127 = call i64 @strlen(ptr %concat.buf24)
  %len228 = call i64 @strlen(ptr %"\D8\A7\D9\84\D9\82\D8\B3\D9\854")
  %totallen29 = add i64 %len127, %len228
  %bufsize30 = add i64 %totallen29, 1
  %concat.buf31 = alloca i8, i64 %bufsize30, align 1
  %4 = call ptr @memcpy(ptr %concat.buf31, ptr %concat.buf24, i64 %len127)
  %dest232 = getelementptr i8, ptr %concat.buf31, i64 %len127
  %copylen233 = add i64 %len228, 1
  %5 = call ptr @memcpy(ptr %dest232, ptr %"\D8\A7\D9\84\D9\82\D8\B3\D9\854", i64 %copylen233)
  %len134 = call i64 @strlen(ptr %concat.buf31)
  %len235 = call i64 @strlen(ptr @str.const.20)
  %totallen36 = add i64 %len134, %len235
  %bufsize37 = add i64 %totallen36, 1
  %concat.buf38 = alloca i8, i64 %bufsize37, align 1
  %6 = call ptr @memcpy(ptr %concat.buf38, ptr %concat.buf31, i64 %len134)
  %dest239 = getelementptr i8, ptr %concat.buf38, i64 %len134
  %copylen240 = add i64 %len235, 1
  %7 = call ptr @memcpy(ptr %dest239, ptr @str.const.20, i64 %copylen240)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  ret void

match.case0.test_1:                               ; preds = %entry
  %cmpeqtmp = icmp eq i64 %loadtmp, 1
  br i1 %cmpeqtmp, label %match.case0.body_2, label %match.case1.test_3

match.case0.body_2:                               ; preds = %match.case0.test_1
  store ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\852", ptr %alloca5, align 8
  %"%\D8\A7\D9\84\D8\B1\D8\A7\D8\AA\D8\A8.load" = load i64, ptr %"\D8\A7\D9\84\D8\B1\D8\A7\D8\AA\D8\A83", align 8
  store i64 %"%\D8\A7\D9\84\D8\B1\D8\A7\D8\AA\D8\A8.load", ptr %alloca11, align 8
  store ptr %"\D8\A7\D9\84\D9\82\D8\B3\D9\854", ptr %alloca15, align 8
  br label %match.end_0

match.case1.test_3:                               ; preds = %match.case0.test_1
  %cmpeqtmp41 = icmp eq i64 %loadtmp, 2
  br i1 %cmpeqtmp41, label %match.case1.body_4, label %match.case2.test_5

match.case1.body_4:                               ; preds = %match.case1.test_3
  store ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\852", ptr %alloca6, align 8
  %"%\D8\A7\D9\84\D8\B1\D8\A7\D8\AA\D8\A8.load42" = load i64, ptr %"\D8\A7\D9\84\D8\B1\D8\A7\D8\AA\D8\A83", align 8
  store i64 %"%\D8\A7\D9\84\D8\B1\D8\A7\D8\AA\D8\A8.load42", ptr %alloca8, align 8
  store ptr %"\D8\A7\D9\84\D9\82\D8\B3\D9\854", ptr %alloca16, align 8
  br label %match.end_0

match.case2.test_5:                               ; preds = %match.case1.test_3
  %cmpeqtmp43 = icmp eq i64 %loadtmp, 3
  br i1 %cmpeqtmp43, label %match.case2.body_6, label %match.case3.test_7

match.case2.body_6:                               ; preds = %match.case2.test_5
  store ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\852", ptr %alloca7, align 8
  %"%\D8\A7\D9\84\D8\B1\D8\A7\D8\AA\D8\A8.load44" = load i64, ptr %"\D8\A7\D9\84\D8\B1\D8\A7\D8\AA\D8\A83", align 8
  store i64 %"%\D8\A7\D9\84\D8\B1\D8\A7\D8\AA\D8\A8.load44", ptr %alloca12, align 8
  store ptr %"\D8\A7\D9\84\D9\82\D8\B3\D9\854", ptr %alloca17, align 8
  br label %match.end_0

match.case3.test_7:                               ; preds = %match.case2.test_5
  %cmpeqtmp45 = icmp eq i64 %loadtmp, 4
  br i1 %cmpeqtmp45, label %match.case3.body_8, label %match.case4.test_9

match.case3.body_8:                               ; preds = %match.case3.test_7
  store ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\852", ptr %alloca9, align 8
  %"%\D8\A7\D9\84\D8\B1\D8\A7\D8\AA\D8\A8.load46" = load i64, ptr %"\D8\A7\D9\84\D8\B1\D8\A7\D8\AA\D8\A83", align 8
  store i64 %"%\D8\A7\D9\84\D8\B1\D8\A7\D8\AA\D8\A8.load46", ptr %alloca13, align 8
  store ptr %"\D8\A7\D9\84\D9\82\D8\B3\D9\854", ptr %alloca18, align 8
  br label %match.end_0

match.case4.test_9:                               ; preds = %match.case3.test_7
  %cmpeqtmp47 = icmp eq i64 %loadtmp, 5
  br i1 %cmpeqtmp47, label %match.case4.body_10, label %match.end_0

match.case4.body_10:                              ; preds = %match.case4.test_9
  store ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\852", ptr %alloca10, align 8
  %"%\D8\A7\D9\84\D8\B1\D8\A7\D8\AA\D8\A8.load48" = load i64, ptr %"\D8\A7\D9\84\D8\B1\D8\A7\D8\AA\D8\A83", align 8
  store i64 %"%\D8\A7\D9\84\D8\B1\D8\A7\D8\AA\D8\A8.load48", ptr %alloca14, align 8
  store ptr %"\D8\A7\D9\84\D9\82\D8\B3\D9\854", ptr %alloca19, align 8
  br label %match.end_0
}

define void @"\D8\AC\D8\AF\D9\88\D9\84_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.\D8\A7\D8\B9\D8\B1\D8\B6_\D8\AC\D8\AF\D9\88\D9\84"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca ptr, align 8
  %alloca4 = alloca ptr, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca ptr, align 8
  %alloca7 = alloca ptr, align 8
  %alloca8 = alloca i64, align 8
  %alloca9 = alloca i64, align 8
  %alloca10 = alloca i64, align 8
  %alloca11 = alloca i64, align 8
  %alloca12 = alloca ptr, align 8
  %alloca13 = alloca ptr, align 8
  %alloca14 = alloca ptr, align 8
  %alloca15 = alloca ptr, align 8
  %alloca16 = alloca ptr, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.24)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.28)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load" = load i64, ptr %alloca, align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load", 0
  br i1 %cmpgttmp, label %then_11, label %merge_12

then_11:                                          ; preds = %entry
  %len1 = call i64 @strlen(ptr @str.const.32)
  %len2 = call i64 @strlen(ptr %alloca2)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %4 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.32, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %5 = call ptr @memcpy(ptr %dest2, ptr %alloca2, i64 %copylen2)
  %len117 = call i64 @strlen(ptr %concat.buf)
  %len218 = call i64 @strlen(ptr @str.const.20)
  %totallen19 = add i64 %len117, %len218
  %bufsize20 = add i64 %totallen19, 1
  %concat.buf21 = alloca i8, i64 %bufsize20, align 1
  %6 = call ptr @memcpy(ptr %concat.buf21, ptr %concat.buf, i64 %len117)
  %dest222 = getelementptr i8, ptr %concat.buf21, i64 %len117
  %copylen223 = add i64 %len218, 1
  %7 = call ptr @memcpy(ptr %dest222, ptr @str.const.20, i64 %copylen223)
  %len124 = call i64 @strlen(ptr %concat.buf21)
  %len225 = call i64 @strlen(ptr %alloca12)
  %totallen26 = add i64 %len124, %len225
  %bufsize27 = add i64 %totallen26, 1
  %concat.buf28 = alloca i8, i64 %bufsize27, align 1
  %8 = call ptr @memcpy(ptr %concat.buf28, ptr %concat.buf21, i64 %len124)
  %dest229 = getelementptr i8, ptr %concat.buf28, i64 %len124
  %copylen230 = add i64 %len225, 1
  %9 = call ptr @memcpy(ptr %dest229, ptr %alloca12, i64 %copylen230)
  %len131 = call i64 @strlen(ptr %concat.buf28)
  %len232 = call i64 @strlen(ptr @str.const.20)
  %totallen33 = add i64 %len131, %len232
  %bufsize34 = add i64 %totallen33, 1
  %concat.buf35 = alloca i8, i64 %bufsize34, align 1
  %10 = call ptr @memcpy(ptr %concat.buf35, ptr %concat.buf28, i64 %len131)
  %dest236 = getelementptr i8, ptr %concat.buf35, i64 %len131
  %copylen237 = add i64 %len232, 1
  %11 = call ptr @memcpy(ptr %dest236, ptr @str.const.20, i64 %copylen237)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  br label %merge_12

merge_12:                                         ; preds = %then_11, %entry
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load38" = load i64, ptr %alloca, align 8
  %cmpgttmp39 = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load38", 1
  br i1 %cmpgttmp39, label %then_13, label %merge_14

then_13:                                          ; preds = %merge_12
  %len140 = call i64 @strlen(ptr @str.const.32)
  %len241 = call i64 @strlen(ptr %alloca3)
  %totallen42 = add i64 %len140, %len241
  %bufsize43 = add i64 %totallen42, 1
  %concat.buf44 = alloca i8, i64 %bufsize43, align 1
  %13 = call ptr @memcpy(ptr %concat.buf44, ptr @str.const.32, i64 %len140)
  %dest245 = getelementptr i8, ptr %concat.buf44, i64 %len140
  %copylen246 = add i64 %len241, 1
  %14 = call ptr @memcpy(ptr %dest245, ptr %alloca3, i64 %copylen246)
  %len147 = call i64 @strlen(ptr %concat.buf44)
  %len248 = call i64 @strlen(ptr @str.const.20)
  %totallen49 = add i64 %len147, %len248
  %bufsize50 = add i64 %totallen49, 1
  %concat.buf51 = alloca i8, i64 %bufsize50, align 1
  %15 = call ptr @memcpy(ptr %concat.buf51, ptr %concat.buf44, i64 %len147)
  %dest252 = getelementptr i8, ptr %concat.buf51, i64 %len147
  %copylen253 = add i64 %len248, 1
  %16 = call ptr @memcpy(ptr %dest252, ptr @str.const.20, i64 %copylen253)
  %len154 = call i64 @strlen(ptr %concat.buf51)
  %len255 = call i64 @strlen(ptr %alloca13)
  %totallen56 = add i64 %len154, %len255
  %bufsize57 = add i64 %totallen56, 1
  %concat.buf58 = alloca i8, i64 %bufsize57, align 1
  %17 = call ptr @memcpy(ptr %concat.buf58, ptr %concat.buf51, i64 %len154)
  %dest259 = getelementptr i8, ptr %concat.buf58, i64 %len154
  %copylen260 = add i64 %len255, 1
  %18 = call ptr @memcpy(ptr %dest259, ptr %alloca13, i64 %copylen260)
  %len161 = call i64 @strlen(ptr %concat.buf58)
  %len262 = call i64 @strlen(ptr @str.const.20)
  %totallen63 = add i64 %len161, %len262
  %bufsize64 = add i64 %totallen63, 1
  %concat.buf65 = alloca i8, i64 %bufsize64, align 1
  %19 = call ptr @memcpy(ptr %concat.buf65, ptr %concat.buf58, i64 %len161)
  %dest266 = getelementptr i8, ptr %concat.buf65, i64 %len161
  %copylen267 = add i64 %len262, 1
  %20 = call ptr @memcpy(ptr %dest266, ptr @str.const.20, i64 %copylen267)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  br label %merge_14

merge_14:                                         ; preds = %then_13, %merge_12
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load68" = load i64, ptr %alloca, align 8
  %cmpgttmp69 = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load68", 2
  br i1 %cmpgttmp69, label %then_15, label %merge_16

then_15:                                          ; preds = %merge_14
  %len170 = call i64 @strlen(ptr @str.const.32)
  %len271 = call i64 @strlen(ptr %alloca4)
  %totallen72 = add i64 %len170, %len271
  %bufsize73 = add i64 %totallen72, 1
  %concat.buf74 = alloca i8, i64 %bufsize73, align 1
  %22 = call ptr @memcpy(ptr %concat.buf74, ptr @str.const.32, i64 %len170)
  %dest275 = getelementptr i8, ptr %concat.buf74, i64 %len170
  %copylen276 = add i64 %len271, 1
  %23 = call ptr @memcpy(ptr %dest275, ptr %alloca4, i64 %copylen276)
  %len177 = call i64 @strlen(ptr %concat.buf74)
  %len278 = call i64 @strlen(ptr @str.const.20)
  %totallen79 = add i64 %len177, %len278
  %bufsize80 = add i64 %totallen79, 1
  %concat.buf81 = alloca i8, i64 %bufsize80, align 1
  %24 = call ptr @memcpy(ptr %concat.buf81, ptr %concat.buf74, i64 %len177)
  %dest282 = getelementptr i8, ptr %concat.buf81, i64 %len177
  %copylen283 = add i64 %len278, 1
  %25 = call ptr @memcpy(ptr %dest282, ptr @str.const.20, i64 %copylen283)
  %len184 = call i64 @strlen(ptr %concat.buf81)
  %len285 = call i64 @strlen(ptr %alloca14)
  %totallen86 = add i64 %len184, %len285
  %bufsize87 = add i64 %totallen86, 1
  %concat.buf88 = alloca i8, i64 %bufsize87, align 1
  %26 = call ptr @memcpy(ptr %concat.buf88, ptr %concat.buf81, i64 %len184)
  %dest289 = getelementptr i8, ptr %concat.buf88, i64 %len184
  %copylen290 = add i64 %len285, 1
  %27 = call ptr @memcpy(ptr %dest289, ptr %alloca14, i64 %copylen290)
  %len191 = call i64 @strlen(ptr %concat.buf88)
  %len292 = call i64 @strlen(ptr @str.const.20)
  %totallen93 = add i64 %len191, %len292
  %bufsize94 = add i64 %totallen93, 1
  %concat.buf95 = alloca i8, i64 %bufsize94, align 1
  %28 = call ptr @memcpy(ptr %concat.buf95, ptr %concat.buf88, i64 %len191)
  %dest296 = getelementptr i8, ptr %concat.buf95, i64 %len191
  %copylen297 = add i64 %len292, 1
  %29 = call ptr @memcpy(ptr %dest296, ptr @str.const.20, i64 %copylen297)
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  br label %merge_16

merge_16:                                         ; preds = %then_15, %merge_14
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load98" = load i64, ptr %alloca, align 8
  %cmpgttmp99 = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load98", 3
  br i1 %cmpgttmp99, label %then_17, label %merge_18

then_17:                                          ; preds = %merge_16
  %len1100 = call i64 @strlen(ptr @str.const.32)
  %len2101 = call i64 @strlen(ptr %alloca6)
  %totallen102 = add i64 %len1100, %len2101
  %bufsize103 = add i64 %totallen102, 1
  %concat.buf104 = alloca i8, i64 %bufsize103, align 1
  %31 = call ptr @memcpy(ptr %concat.buf104, ptr @str.const.32, i64 %len1100)
  %dest2105 = getelementptr i8, ptr %concat.buf104, i64 %len1100
  %copylen2106 = add i64 %len2101, 1
  %32 = call ptr @memcpy(ptr %dest2105, ptr %alloca6, i64 %copylen2106)
  %len1107 = call i64 @strlen(ptr %concat.buf104)
  %len2108 = call i64 @strlen(ptr @str.const.20)
  %totallen109 = add i64 %len1107, %len2108
  %bufsize110 = add i64 %totallen109, 1
  %concat.buf111 = alloca i8, i64 %bufsize110, align 1
  %33 = call ptr @memcpy(ptr %concat.buf111, ptr %concat.buf104, i64 %len1107)
  %dest2112 = getelementptr i8, ptr %concat.buf111, i64 %len1107
  %copylen2113 = add i64 %len2108, 1
  %34 = call ptr @memcpy(ptr %dest2112, ptr @str.const.20, i64 %copylen2113)
  %len1114 = call i64 @strlen(ptr %concat.buf111)
  %len2115 = call i64 @strlen(ptr %alloca15)
  %totallen116 = add i64 %len1114, %len2115
  %bufsize117 = add i64 %totallen116, 1
  %concat.buf118 = alloca i8, i64 %bufsize117, align 1
  %35 = call ptr @memcpy(ptr %concat.buf118, ptr %concat.buf111, i64 %len1114)
  %dest2119 = getelementptr i8, ptr %concat.buf118, i64 %len1114
  %copylen2120 = add i64 %len2115, 1
  %36 = call ptr @memcpy(ptr %dest2119, ptr %alloca15, i64 %copylen2120)
  %len1121 = call i64 @strlen(ptr %concat.buf118)
  %len2122 = call i64 @strlen(ptr @str.const.20)
  %totallen123 = add i64 %len1121, %len2122
  %bufsize124 = add i64 %totallen123, 1
  %concat.buf125 = alloca i8, i64 %bufsize124, align 1
  %37 = call ptr @memcpy(ptr %concat.buf125, ptr %concat.buf118, i64 %len1121)
  %dest2126 = getelementptr i8, ptr %concat.buf125, i64 %len1121
  %copylen2127 = add i64 %len2122, 1
  %38 = call ptr @memcpy(ptr %dest2126, ptr @str.const.20, i64 %copylen2127)
  %39 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  br label %merge_18

merge_18:                                         ; preds = %then_17, %merge_16
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load128" = load i64, ptr %alloca, align 8
  %cmpgttmp129 = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load128", 4
  br i1 %cmpgttmp129, label %then_19, label %merge_20

then_19:                                          ; preds = %merge_18
  %len1130 = call i64 @strlen(ptr @str.const.32)
  %len2131 = call i64 @strlen(ptr %alloca7)
  %totallen132 = add i64 %len1130, %len2131
  %bufsize133 = add i64 %totallen132, 1
  %concat.buf134 = alloca i8, i64 %bufsize133, align 1
  %40 = call ptr @memcpy(ptr %concat.buf134, ptr @str.const.32, i64 %len1130)
  %dest2135 = getelementptr i8, ptr %concat.buf134, i64 %len1130
  %copylen2136 = add i64 %len2131, 1
  %41 = call ptr @memcpy(ptr %dest2135, ptr %alloca7, i64 %copylen2136)
  %len1137 = call i64 @strlen(ptr %concat.buf134)
  %len2138 = call i64 @strlen(ptr @str.const.20)
  %totallen139 = add i64 %len1137, %len2138
  %bufsize140 = add i64 %totallen139, 1
  %concat.buf141 = alloca i8, i64 %bufsize140, align 1
  %42 = call ptr @memcpy(ptr %concat.buf141, ptr %concat.buf134, i64 %len1137)
  %dest2142 = getelementptr i8, ptr %concat.buf141, i64 %len1137
  %copylen2143 = add i64 %len2138, 1
  %43 = call ptr @memcpy(ptr %dest2142, ptr @str.const.20, i64 %copylen2143)
  %len1144 = call i64 @strlen(ptr %concat.buf141)
  %len2145 = call i64 @strlen(ptr %alloca16)
  %totallen146 = add i64 %len1144, %len2145
  %bufsize147 = add i64 %totallen146, 1
  %concat.buf148 = alloca i8, i64 %bufsize147, align 1
  %44 = call ptr @memcpy(ptr %concat.buf148, ptr %concat.buf141, i64 %len1144)
  %dest2149 = getelementptr i8, ptr %concat.buf148, i64 %len1144
  %copylen2150 = add i64 %len2145, 1
  %45 = call ptr @memcpy(ptr %dest2149, ptr %alloca16, i64 %copylen2150)
  %len1151 = call i64 @strlen(ptr %concat.buf148)
  %len2152 = call i64 @strlen(ptr @str.const.20)
  %totallen153 = add i64 %len1151, %len2152
  %bufsize154 = add i64 %totallen153, 1
  %concat.buf155 = alloca i8, i64 %bufsize154, align 1
  %46 = call ptr @memcpy(ptr %concat.buf155, ptr %concat.buf148, i64 %len1151)
  %dest2156 = getelementptr i8, ptr %concat.buf155, i64 %len1151
  %copylen2157 = add i64 %len2152, 1
  %47 = call ptr @memcpy(ptr %dest2156, ptr @str.const.20, i64 %copylen2157)
  %48 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  br label %merge_20

merge_20:                                         ; preds = %then_19, %merge_18
  %49 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  ret void
}

define i64 @"\D8\AC\D8\AF\D9\88\D9\84_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.\D9\85\D8\AC\D9\85\D9\88\D8\B9_\D8\B1\D9\88\D8\A7\D8\AA\D8\A8"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca ptr, align 8
  %alloca4 = alloca ptr, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca ptr, align 8
  %alloca7 = alloca ptr, align 8
  %alloca8 = alloca i64, align 8
  %alloca9 = alloca i64, align 8
  %alloca10 = alloca i64, align 8
  %alloca11 = alloca i64, align 8
  %alloca12 = alloca ptr, align 8
  %alloca13 = alloca ptr, align 8
  %alloca14 = alloca ptr, align 8
  %alloca15 = alloca ptr, align 8
  %alloca16 = alloca ptr, align 8
  %alloca17 = alloca i64, align 8
  store i64 0, ptr %alloca17, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load" = load i64, ptr %alloca, align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load", 0
  br i1 %cmpgttmp, label %then_21, label %merge_22

then_21:                                          ; preds = %entry
  %"%\D9\85.load" = load i64, ptr %alloca17, align 8
  %"%\D8\B1\D8\A7\D8\AA\D8\A81.load" = load i64, ptr %alloca8, align 8
  %addtmp = add i64 %"%\D9\85.load", %"%\D8\B1\D8\A7\D8\AA\D8\A81.load"
  store i64 %addtmp, ptr %alloca17, align 8
  br label %merge_22

merge_22:                                         ; preds = %then_21, %entry
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load18" = load i64, ptr %alloca, align 8
  %cmpgttmp19 = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load18", 1
  br i1 %cmpgttmp19, label %then_23, label %merge_24

then_23:                                          ; preds = %merge_22
  %"%\D9\85.load20" = load i64, ptr %alloca17, align 8
  %"%\D8\B1\D8\A7\D8\AA\D8\A82.load" = load i64, ptr %alloca5, align 8
  %addtmp21 = add i64 %"%\D9\85.load20", %"%\D8\B1\D8\A7\D8\AA\D8\A82.load"
  store i64 %addtmp21, ptr %alloca17, align 8
  br label %merge_24

merge_24:                                         ; preds = %then_23, %merge_22
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load22" = load i64, ptr %alloca, align 8
  %cmpgttmp23 = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load22", 2
  br i1 %cmpgttmp23, label %then_25, label %merge_26

then_25:                                          ; preds = %merge_24
  %"%\D9\85.load24" = load i64, ptr %alloca17, align 8
  %"%\D8\B1\D8\A7\D8\AA\D8\A83.load" = load i64, ptr %alloca9, align 8
  %addtmp25 = add i64 %"%\D9\85.load24", %"%\D8\B1\D8\A7\D8\AA\D8\A83.load"
  store i64 %addtmp25, ptr %alloca17, align 8
  br label %merge_26

merge_26:                                         ; preds = %then_25, %merge_24
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load26" = load i64, ptr %alloca, align 8
  %cmpgttmp27 = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load26", 3
  br i1 %cmpgttmp27, label %then_27, label %merge_28

then_27:                                          ; preds = %merge_26
  %"%\D9\85.load28" = load i64, ptr %alloca17, align 8
  %"%\D8\B1\D8\A7\D8\AA\D8\A84.load" = load i64, ptr %alloca10, align 8
  %addtmp29 = add i64 %"%\D9\85.load28", %"%\D8\B1\D8\A7\D8\AA\D8\A84.load"
  store i64 %addtmp29, ptr %alloca17, align 8
  br label %merge_28

merge_28:                                         ; preds = %then_27, %merge_26
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load30" = load i64, ptr %alloca, align 8
  %cmpgttmp31 = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load30", 4
  br i1 %cmpgttmp31, label %then_29, label %merge_30

then_29:                                          ; preds = %merge_28
  %"%\D9\85.load32" = load i64, ptr %alloca17, align 8
  %"%\D8\B1\D8\A7\D8\AA\D8\A85.load" = load i64, ptr %alloca11, align 8
  %addtmp33 = add i64 %"%\D9\85.load32", %"%\D8\B1\D8\A7\D8\AA\D8\A85.load"
  store i64 %addtmp33, ptr %alloca17, align 8
  br label %merge_30

merge_30:                                         ; preds = %then_29, %merge_28
  %loadtmp = load i64, ptr %alloca17, align 8
  ret i64 %loadtmp
}

define i64 @"\D8\AC\D8\AF\D9\88\D9\84_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.\D9\85\D8\AA\D9\88\D8\B3\D8\B7_\D8\B1\D9\88\D8\A7\D8\AA\D8\A8"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca ptr, align 8
  %alloca4 = alloca ptr, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca ptr, align 8
  %alloca7 = alloca ptr, align 8
  %alloca8 = alloca i64, align 8
  %alloca9 = alloca i64, align 8
  %alloca10 = alloca i64, align 8
  %alloca11 = alloca i64, align 8
  %alloca12 = alloca ptr, align 8
  %alloca13 = alloca ptr, align 8
  %alloca14 = alloca ptr, align 8
  %alloca15 = alloca ptr, align 8
  %alloca16 = alloca ptr, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load" = load i64, ptr %alloca, align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load", 0
  br i1 %cmpeqtmp, label %then_31, label %merge_32

then_31:                                          ; preds = %entry
  ret i64 0

merge_32:                                         ; preds = %entry
  call void @"\D9\85\D8\AC\D9\85\D9\88\D8\B9_\D8\B1\D9\88\D8\A7\D8\AA\D8\A8"()
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load17" = load i64, ptr %alloca, align 8
  %divtmp = sdiv i64 0, %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load17"
  ret i64 %divtmp
}

define i64 @"\D8\AC\D8\AF\D9\88\D9\84_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.\D8\A3\D8\B9\D9\84\D9\89_\D8\B1\D8\A7\D8\AA\D8\A8"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca ptr, align 8
  %alloca4 = alloca ptr, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca ptr, align 8
  %alloca7 = alloca ptr, align 8
  %alloca8 = alloca i64, align 8
  %alloca9 = alloca i64, align 8
  %alloca10 = alloca i64, align 8
  %alloca11 = alloca i64, align 8
  %alloca12 = alloca ptr, align 8
  %alloca13 = alloca ptr, align 8
  %alloca14 = alloca ptr, align 8
  %alloca15 = alloca ptr, align 8
  %alloca16 = alloca ptr, align 8
  %alloca17 = alloca i64, align 8
  store i64 0, ptr %alloca17, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load" = load i64, ptr %alloca, align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load", 0
  br i1 %cmpgttmp, label %then_33, label %merge_34

then_33:                                          ; preds = %entry
  %"%\D8\B1\D8\A7\D8\AA\D8\A81.load" = load i64, ptr %alloca8, align 8
  store i64 %"%\D8\B1\D8\A7\D8\AA\D8\A81.load", ptr %alloca17, align 8
  br label %merge_34

merge_34:                                         ; preds = %then_33, %entry
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load18" = load i64, ptr %alloca, align 8
  %cmpgttmp19 = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load18", 1
  br i1 %cmpgttmp19, label %then_35, label %merge_36

then_35:                                          ; preds = %merge_34
  %"%\D8\B1\D8\A7\D8\AA\D8\A82.load" = load i64, ptr %alloca5, align 8
  %"%\D8\A3\D8\B9\D9\84\D9\89.load" = load i64, ptr %alloca17, align 8
  %cmpgttmp20 = icmp sgt i64 %"%\D8\B1\D8\A7\D8\AA\D8\A82.load", %"%\D8\A3\D8\B9\D9\84\D9\89.load"
  br i1 %cmpgttmp20, label %then_37, label %merge_38

merge_36:                                         ; preds = %merge_38, %merge_34
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load21" = load i64, ptr %alloca, align 8
  %cmpgttmp22 = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load21", 2
  br i1 %cmpgttmp22, label %then_39, label %merge_40

then_37:                                          ; preds = %then_35
  %"%\D8\B1\D8\A7\D8\AA\D8\A82.load23" = load i64, ptr %alloca5, align 8
  store i64 %"%\D8\B1\D8\A7\D8\AA\D8\A82.load23", ptr %alloca17, align 8
  br label %merge_38

merge_38:                                         ; preds = %then_37, %then_35
  br label %merge_36

then_39:                                          ; preds = %merge_36
  %"%\D8\B1\D8\A7\D8\AA\D8\A83.load" = load i64, ptr %alloca9, align 8
  %"%\D8\A3\D8\B9\D9\84\D9\89.load24" = load i64, ptr %alloca17, align 8
  %cmpgttmp25 = icmp sgt i64 %"%\D8\B1\D8\A7\D8\AA\D8\A83.load", %"%\D8\A3\D8\B9\D9\84\D9\89.load24"
  br i1 %cmpgttmp25, label %then_41, label %merge_42

merge_40:                                         ; preds = %merge_42, %merge_36
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load26" = load i64, ptr %alloca, align 8
  %cmpgttmp27 = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load26", 3
  br i1 %cmpgttmp27, label %then_43, label %merge_44

then_41:                                          ; preds = %then_39
  %"%\D8\B1\D8\A7\D8\AA\D8\A83.load28" = load i64, ptr %alloca9, align 8
  store i64 %"%\D8\B1\D8\A7\D8\AA\D8\A83.load28", ptr %alloca17, align 8
  br label %merge_42

merge_42:                                         ; preds = %then_41, %then_39
  br label %merge_40

then_43:                                          ; preds = %merge_40
  %"%\D8\B1\D8\A7\D8\AA\D8\A84.load" = load i64, ptr %alloca10, align 8
  %"%\D8\A3\D8\B9\D9\84\D9\89.load29" = load i64, ptr %alloca17, align 8
  %cmpgttmp30 = icmp sgt i64 %"%\D8\B1\D8\A7\D8\AA\D8\A84.load", %"%\D8\A3\D8\B9\D9\84\D9\89.load29"
  br i1 %cmpgttmp30, label %then_45, label %merge_46

merge_44:                                         ; preds = %merge_46, %merge_40
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load31" = load i64, ptr %alloca, align 8
  %cmpgttmp32 = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load31", 4
  br i1 %cmpgttmp32, label %then_47, label %merge_48

then_45:                                          ; preds = %then_43
  %"%\D8\B1\D8\A7\D8\AA\D8\A84.load33" = load i64, ptr %alloca10, align 8
  store i64 %"%\D8\B1\D8\A7\D8\AA\D8\A84.load33", ptr %alloca17, align 8
  br label %merge_46

merge_46:                                         ; preds = %then_45, %then_43
  br label %merge_44

then_47:                                          ; preds = %merge_44
  %"%\D8\B1\D8\A7\D8\AA\D8\A85.load" = load i64, ptr %alloca11, align 8
  %"%\D8\A3\D8\B9\D9\84\D9\89.load34" = load i64, ptr %alloca17, align 8
  %cmpgttmp35 = icmp sgt i64 %"%\D8\B1\D8\A7\D8\AA\D8\A85.load", %"%\D8\A3\D8\B9\D9\84\D9\89.load34"
  br i1 %cmpgttmp35, label %then_49, label %merge_50

merge_48:                                         ; preds = %merge_50, %merge_44
  %loadtmp = load i64, ptr %alloca17, align 8
  ret i64 %loadtmp

then_49:                                          ; preds = %then_47
  %"%\D8\B1\D8\A7\D8\AA\D8\A85.load36" = load i64, ptr %alloca11, align 8
  store i64 %"%\D8\B1\D8\A7\D8\AA\D8\A85.load36", ptr %alloca17, align 8
  br label %merge_50

merge_50:                                         ; preds = %then_49, %then_47
  br label %merge_48
}

define i64 @"\D8\AC\D8\AF\D9\88\D9\84_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.\D8\A3\D8\AF\D9\86\D9\89_\D8\B1\D8\A7\D8\AA\D8\A8"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca ptr, align 8
  %alloca4 = alloca ptr, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca ptr, align 8
  %alloca7 = alloca ptr, align 8
  %alloca8 = alloca i64, align 8
  %alloca9 = alloca i64, align 8
  %alloca10 = alloca i64, align 8
  %alloca11 = alloca i64, align 8
  %alloca12 = alloca ptr, align 8
  %alloca13 = alloca ptr, align 8
  %alloca14 = alloca ptr, align 8
  %alloca15 = alloca ptr, align 8
  %alloca16 = alloca ptr, align 8
  %alloca17 = alloca i64, align 8
  store i64 999999, ptr %alloca17, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load" = load i64, ptr %alloca, align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load", 0
  br i1 %cmpgttmp, label %then_51, label %merge_52

then_51:                                          ; preds = %entry
  %"%\D8\B1\D8\A7\D8\AA\D8\A81.load" = load i64, ptr %alloca8, align 8
  store i64 %"%\D8\B1\D8\A7\D8\AA\D8\A81.load", ptr %alloca17, align 8
  br label %merge_52

merge_52:                                         ; preds = %then_51, %entry
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load18" = load i64, ptr %alloca, align 8
  %cmpgttmp19 = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load18", 1
  br i1 %cmpgttmp19, label %then_53, label %merge_54

then_53:                                          ; preds = %merge_52
  %"%\D8\B1\D8\A7\D8\AA\D8\A82.load" = load i64, ptr %alloca5, align 8
  %"%\D8\A3\D8\AF\D9\86\D9\89.load" = load i64, ptr %alloca17, align 8
  %cmplttmp = icmp slt i64 %"%\D8\B1\D8\A7\D8\AA\D8\A82.load", %"%\D8\A3\D8\AF\D9\86\D9\89.load"
  br i1 %cmplttmp, label %then_55, label %merge_56

merge_54:                                         ; preds = %merge_56, %merge_52
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load20" = load i64, ptr %alloca, align 8
  %cmpgttmp21 = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load20", 2
  br i1 %cmpgttmp21, label %then_57, label %merge_58

then_55:                                          ; preds = %then_53
  %"%\D8\B1\D8\A7\D8\AA\D8\A82.load22" = load i64, ptr %alloca5, align 8
  store i64 %"%\D8\B1\D8\A7\D8\AA\D8\A82.load22", ptr %alloca17, align 8
  br label %merge_56

merge_56:                                         ; preds = %then_55, %then_53
  br label %merge_54

then_57:                                          ; preds = %merge_54
  %"%\D8\B1\D8\A7\D8\AA\D8\A83.load" = load i64, ptr %alloca9, align 8
  %"%\D8\A3\D8\AF\D9\86\D9\89.load23" = load i64, ptr %alloca17, align 8
  %cmplttmp24 = icmp slt i64 %"%\D8\B1\D8\A7\D8\AA\D8\A83.load", %"%\D8\A3\D8\AF\D9\86\D9\89.load23"
  br i1 %cmplttmp24, label %then_59, label %merge_60

merge_58:                                         ; preds = %merge_60, %merge_54
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load25" = load i64, ptr %alloca, align 8
  %cmpgttmp26 = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load25", 3
  br i1 %cmpgttmp26, label %then_61, label %merge_62

then_59:                                          ; preds = %then_57
  %"%\D8\B1\D8\A7\D8\AA\D8\A83.load27" = load i64, ptr %alloca9, align 8
  store i64 %"%\D8\B1\D8\A7\D8\AA\D8\A83.load27", ptr %alloca17, align 8
  br label %merge_60

merge_60:                                         ; preds = %then_59, %then_57
  br label %merge_58

then_61:                                          ; preds = %merge_58
  %"%\D8\B1\D8\A7\D8\AA\D8\A84.load" = load i64, ptr %alloca10, align 8
  %"%\D8\A3\D8\AF\D9\86\D9\89.load28" = load i64, ptr %alloca17, align 8
  %cmplttmp29 = icmp slt i64 %"%\D8\B1\D8\A7\D8\AA\D8\A84.load", %"%\D8\A3\D8\AF\D9\86\D9\89.load28"
  br i1 %cmplttmp29, label %then_63, label %merge_64

merge_62:                                         ; preds = %merge_64, %merge_58
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load30" = load i64, ptr %alloca, align 8
  %cmpgttmp31 = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.load30", 4
  br i1 %cmpgttmp31, label %then_65, label %merge_66

then_63:                                          ; preds = %then_61
  %"%\D8\B1\D8\A7\D8\AA\D8\A84.load32" = load i64, ptr %alloca10, align 8
  store i64 %"%\D8\B1\D8\A7\D8\AA\D8\A84.load32", ptr %alloca17, align 8
  br label %merge_64

merge_64:                                         ; preds = %then_63, %then_61
  br label %merge_62

then_65:                                          ; preds = %merge_62
  %"%\D8\B1\D8\A7\D8\AA\D8\A85.load" = load i64, ptr %alloca11, align 8
  %"%\D8\A3\D8\AF\D9\86\D9\89.load33" = load i64, ptr %alloca17, align 8
  %cmplttmp34 = icmp slt i64 %"%\D8\B1\D8\A7\D8\AA\D8\A85.load", %"%\D8\A3\D8\AF\D9\86\D9\89.load33"
  br i1 %cmplttmp34, label %then_67, label %merge_68

merge_66:                                         ; preds = %merge_68, %merge_62
  %loadtmp = load i64, ptr %alloca17, align 8
  ret i64 %loadtmp

then_67:                                          ; preds = %then_65
  %"%\D8\B1\D8\A7\D8\AA\D8\A85.load35" = load i64, ptr %alloca11, align 8
  store i64 %"%\D8\B1\D8\A7\D8\AA\D8\A85.load35", ptr %alloca17, align 8
  br label %merge_68

merge_68:                                         ; preds = %then_67, %then_65
  br label %merge_66
}

define void @"\D8\AC\D8\AF\D9\88\D9\84_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.\D8\A7\D8\B9\D8\B1\D8\B6_\D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca ptr, align 8
  %alloca4 = alloca ptr, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca ptr, align 8
  %alloca7 = alloca ptr, align 8
  %alloca8 = alloca i64, align 8
  %alloca9 = alloca i64, align 8
  %alloca10 = alloca i64, align 8
  %alloca11 = alloca i64, align 8
  %alloca12 = alloca ptr, align 8
  %alloca13 = alloca ptr, align 8
  %alloca14 = alloca ptr, align 8
  %alloca15 = alloca ptr, align 8
  %alloca16 = alloca ptr, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.66)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  call void @"\D9\85\D8\AC\D9\85\D9\88\D8\B9_\D8\B1\D9\88\D8\A7\D8\AA\D8\A8"()
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  call void @"\D9\85\D8\AA\D9\88\D8\B3\D8\B7_\D8\B1\D9\88\D8\A7\D8\AA\D8\A8"()
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  call void @"\D8\A3\D8\B9\D9\84\D9\89_\D8\B1\D8\A7\D8\AA\D8\A8"()
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  call void @"\D8\A3\D8\AF\D9\86\D9\89_\D8\B1\D8\A7\D8\AA\D8\A8"()
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  ret void
}

define void @"\D8\AC\D8\AF\D9\88\D9\84_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca2 = alloca i64, align 8
  %alloca3 = alloca ptr, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca ptr, align 8
  %alloca7 = alloca ptr, align 8
  %alloca8 = alloca ptr, align 8
  %alloca9 = alloca ptr, align 8
  %alloca10 = alloca i64, align 8
  %alloca11 = alloca i64, align 8
  %alloca12 = alloca i64, align 8
  %alloca13 = alloca i64, align 8
  store i64 0, ptr %alloca4, align 8
  store ptr @str.const.82, ptr %alloca7, align 8
  store ptr @str.const.83, ptr %alloca9, align 8
  store ptr @str.const.84, ptr %alloca3, align 8
  store ptr @str.const.85, ptr %alloca6, align 8
  store ptr @str.const.86, ptr %alloca8, align 8
  store ptr @str.const.87, ptr %alloca, align 8
  store i64 0, ptr %alloca2, align 8
  store i64 0, ptr %alloca10, align 8
  store i64 0, ptr %alloca5, align 8
  store i64 0, ptr %alloca11, align 8
  store i64 0, ptr %alloca12, align 8
  store i64 0, ptr %alloca13, align 8
  ret void
}

define void @"\D8\AC\D8\AF\D9\88\D9\84_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.\D8\A3\D8\B6\D9\81_\D9\85\D8\B4\D8\B1\D9\88\D8\B9"(i64 %self, ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85", ptr %"\D8\A7\D9\84\D8\AD\D8\A7\D9\84", i64 %"\D8\A7\D9\84\D9\85\D9\8A\D8\B2\D8\A7\D9\86\D9\8A\D8\A9", i64 %"\D8\A7\D9\84\D8\AA\D9\82\D8\AF\D9\85") {
entry:
  %"\D8\A7\D9\84\D8\AA\D9\82\D8\AF\D9\855" = alloca i64, align 8
  store i64 %"\D8\A7\D9\84\D8\AA\D9\82\D8\AF\D9\85", ptr %"\D8\A7\D9\84\D8\AA\D9\82\D8\AF\D9\855", align 8
  %"\D8\A7\D9\84\D9\85\D9\8A\D8\B2\D8\A7\D9\86\D9\8A\D8\A94" = alloca i64, align 8
  store i64 %"\D8\A7\D9\84\D9\85\D9\8A\D8\B2\D8\A7\D9\86\D9\8A\D8\A9", ptr %"\D8\A7\D9\84\D9\85\D9\8A\D8\B2\D8\A7\D9\86\D9\8A\D8\A94", align 8
  %"\D8\A7\D9\84\D8\AD\D8\A7\D9\843" = alloca ptr, align 8
  store ptr %"\D8\A7\D9\84\D8\AD\D8\A7\D9\84", ptr %"\D8\A7\D9\84\D8\AD\D8\A7\D9\843", align 8
  %"\D8\A7\D9\84\D8\A7\D8\B3\D9\852" = alloca ptr, align 8
  store ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85", ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\852", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca6 = alloca i64, align 8
  %alloca7 = alloca ptr, align 8
  %alloca8 = alloca i64, align 8
  %alloca9 = alloca i64, align 8
  %alloca10 = alloca ptr, align 8
  %alloca11 = alloca ptr, align 8
  %alloca12 = alloca ptr, align 8
  %alloca13 = alloca ptr, align 8
  %alloca14 = alloca i64, align 8
  %alloca15 = alloca i64, align 8
  %alloca16 = alloca i64, align 8
  %alloca17 = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load" = load i64, ptr %alloca8, align 8
  %addtmp = add i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load", 1
  store i64 %addtmp, ptr %alloca8, align 8
  %loadtmp = load i64, ptr %alloca8, align 8
  br label %match.case0.test_70

match.end_69:                                     ; preds = %match.case2.body_75, %match.case2.test_74, %match.case1.body_73, %match.case0.body_71
  %len1 = call i64 @strlen(ptr @str.const.88)
  %len2 = call i64 @strlen(ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\852")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.88, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\852", i64 %copylen2)
  %len118 = call i64 @strlen(ptr %concat.buf)
  %len219 = call i64 @strlen(ptr @str.const.20)
  %totallen20 = add i64 %len118, %len219
  %bufsize21 = add i64 %totallen20, 1
  %concat.buf22 = alloca i8, i64 %bufsize21, align 1
  %2 = call ptr @memcpy(ptr %concat.buf22, ptr %concat.buf, i64 %len118)
  %dest223 = getelementptr i8, ptr %concat.buf22, i64 %len118
  %copylen224 = add i64 %len219, 1
  %3 = call ptr @memcpy(ptr %dest223, ptr @str.const.20, i64 %copylen224)
  %len125 = call i64 @strlen(ptr %concat.buf22)
  %len226 = call i64 @strlen(ptr %"\D8\A7\D9\84\D8\AD\D8\A7\D9\843")
  %totallen27 = add i64 %len125, %len226
  %bufsize28 = add i64 %totallen27, 1
  %concat.buf29 = alloca i8, i64 %bufsize28, align 1
  %4 = call ptr @memcpy(ptr %concat.buf29, ptr %concat.buf22, i64 %len125)
  %dest230 = getelementptr i8, ptr %concat.buf29, i64 %len125
  %copylen231 = add i64 %len226, 1
  %5 = call ptr @memcpy(ptr %dest230, ptr %"\D8\A7\D9\84\D8\AD\D8\A7\D9\843", i64 %copylen231)
  %len132 = call i64 @strlen(ptr %concat.buf29)
  %len233 = call i64 @strlen(ptr @str.const.20)
  %totallen34 = add i64 %len132, %len233
  %bufsize35 = add i64 %totallen34, 1
  %concat.buf36 = alloca i8, i64 %bufsize35, align 1
  %6 = call ptr @memcpy(ptr %concat.buf36, ptr %concat.buf29, i64 %len132)
  %dest237 = getelementptr i8, ptr %concat.buf36, i64 %len132
  %copylen238 = add i64 %len233, 1
  %7 = call ptr @memcpy(ptr %dest237, ptr @str.const.20, i64 %copylen238)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  ret void

match.case0.test_70:                              ; preds = %entry
  %cmpeqtmp = icmp eq i64 %loadtmp, 1
  br i1 %cmpeqtmp, label %match.case0.body_71, label %match.case1.test_72

match.case0.body_71:                              ; preds = %match.case0.test_70
  store ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\852", ptr %alloca11, align 8
  store ptr %"\D8\A7\D9\84\D8\AD\D8\A7\D9\843", ptr %alloca10, align 8
  %"%\D8\A7\D9\84\D9\85\D9\8A\D8\B2\D8\A7\D9\86\D9\8A\D8\A9.load" = load i64, ptr %"\D8\A7\D9\84\D9\85\D9\8A\D8\B2\D8\A7\D9\86\D9\8A\D8\A94", align 8
  store i64 %"%\D8\A7\D9\84\D9\85\D9\8A\D8\B2\D8\A7\D9\86\D9\8A\D8\A9.load", ptr %alloca6, align 8
  %"%\D8\A7\D9\84\D8\AA\D9\82\D8\AF\D9\85.load" = load i64, ptr %"\D8\A7\D9\84\D8\AA\D9\82\D8\AF\D9\855", align 8
  store i64 %"%\D8\A7\D9\84\D8\AA\D9\82\D8\AF\D9\85.load", ptr %alloca15, align 8
  br label %match.end_69

match.case1.test_72:                              ; preds = %match.case0.test_70
  %cmpeqtmp39 = icmp eq i64 %loadtmp, 2
  br i1 %cmpeqtmp39, label %match.case1.body_73, label %match.case2.test_74

match.case1.body_73:                              ; preds = %match.case1.test_72
  store ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\852", ptr %alloca13, align 8
  store ptr %"\D8\A7\D9\84\D8\AD\D8\A7\D9\843", ptr %alloca12, align 8
  %"%\D8\A7\D9\84\D9\85\D9\8A\D8\B2\D8\A7\D9\86\D9\8A\D8\A9.load40" = load i64, ptr %"\D8\A7\D9\84\D9\85\D9\8A\D8\B2\D8\A7\D9\86\D9\8A\D8\A94", align 8
  store i64 %"%\D8\A7\D9\84\D9\85\D9\8A\D8\B2\D8\A7\D9\86\D9\8A\D8\A9.load40", ptr %alloca14, align 8
  %"%\D8\A7\D9\84\D8\AA\D9\82\D8\AF\D9\85.load41" = load i64, ptr %"\D8\A7\D9\84\D8\AA\D9\82\D8\AF\D9\855", align 8
  store i64 %"%\D8\A7\D9\84\D8\AA\D9\82\D8\AF\D9\85.load41", ptr %alloca16, align 8
  br label %match.end_69

match.case2.test_74:                              ; preds = %match.case1.test_72
  %cmpeqtmp42 = icmp eq i64 %loadtmp, 3
  br i1 %cmpeqtmp42, label %match.case2.body_75, label %match.end_69

match.case2.body_75:                              ; preds = %match.case2.test_74
  store ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\852", ptr %alloca7, align 8
  store ptr %"\D8\A7\D9\84\D8\AD\D8\A7\D9\843", ptr %alloca, align 8
  %"%\D8\A7\D9\84\D9\85\D9\8A\D8\B2\D8\A7\D9\86\D9\8A\D8\A9.load43" = load i64, ptr %"\D8\A7\D9\84\D9\85\D9\8A\D8\B2\D8\A7\D9\86\D9\8A\D8\A94", align 8
  store i64 %"%\D8\A7\D9\84\D9\85\D9\8A\D8\B2\D8\A7\D9\86\D9\8A\D8\A9.load43", ptr %alloca9, align 8
  %"%\D8\A7\D9\84\D8\AA\D9\82\D8\AF\D9\85.load44" = load i64, ptr %"\D8\A7\D9\84\D8\AA\D9\82\D8\AF\D9\855", align 8
  store i64 %"%\D8\A7\D9\84\D8\AA\D9\82\D8\AF\D9\85.load44", ptr %alloca17, align 8
  br label %match.end_69
}

define void @"\D8\AC\D8\AF\D9\88\D9\84_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.\D8\A7\D8\B9\D8\B1\D8\B6_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca2 = alloca i64, align 8
  %alloca3 = alloca ptr, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca ptr, align 8
  %alloca7 = alloca ptr, align 8
  %alloca8 = alloca ptr, align 8
  %alloca9 = alloca ptr, align 8
  %alloca10 = alloca i64, align 8
  %alloca11 = alloca i64, align 8
  %alloca12 = alloca i64, align 8
  %alloca13 = alloca i64, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.95)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.99)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load" = load i64, ptr %alloca4, align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load", 0
  br i1 %cmpgttmp, label %then_76, label %merge_77

then_76:                                          ; preds = %entry
  %len1 = call i64 @strlen(ptr @str.const.32)
  %len2 = call i64 @strlen(ptr %alloca7)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %4 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.32, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %5 = call ptr @memcpy(ptr %dest2, ptr %alloca7, i64 %copylen2)
  %len114 = call i64 @strlen(ptr %concat.buf)
  %len215 = call i64 @strlen(ptr @str.const.20)
  %totallen16 = add i64 %len114, %len215
  %bufsize17 = add i64 %totallen16, 1
  %concat.buf18 = alloca i8, i64 %bufsize17, align 1
  %6 = call ptr @memcpy(ptr %concat.buf18, ptr %concat.buf, i64 %len114)
  %dest219 = getelementptr i8, ptr %concat.buf18, i64 %len114
  %copylen220 = add i64 %len215, 1
  %7 = call ptr @memcpy(ptr %dest219, ptr @str.const.20, i64 %copylen220)
  %len121 = call i64 @strlen(ptr %concat.buf18)
  %len222 = call i64 @strlen(ptr %alloca6)
  %totallen23 = add i64 %len121, %len222
  %bufsize24 = add i64 %totallen23, 1
  %concat.buf25 = alloca i8, i64 %bufsize24, align 1
  %8 = call ptr @memcpy(ptr %concat.buf25, ptr %concat.buf18, i64 %len121)
  %dest226 = getelementptr i8, ptr %concat.buf25, i64 %len121
  %copylen227 = add i64 %len222, 1
  %9 = call ptr @memcpy(ptr %dest226, ptr %alloca6, i64 %copylen227)
  %len128 = call i64 @strlen(ptr %concat.buf25)
  %len229 = call i64 @strlen(ptr @str.const.20)
  %totallen30 = add i64 %len128, %len229
  %bufsize31 = add i64 %totallen30, 1
  %concat.buf32 = alloca i8, i64 %bufsize31, align 1
  %10 = call ptr @memcpy(ptr %concat.buf32, ptr %concat.buf25, i64 %len128)
  %dest233 = getelementptr i8, ptr %concat.buf32, i64 %len128
  %copylen234 = add i64 %len229, 1
  %11 = call ptr @memcpy(ptr %dest233, ptr @str.const.20, i64 %copylen234)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  br label %merge_77

merge_77:                                         ; preds = %then_76, %entry
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load35" = load i64, ptr %alloca4, align 8
  %cmpgttmp36 = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load35", 1
  br i1 %cmpgttmp36, label %then_78, label %merge_79

then_78:                                          ; preds = %merge_77
  %len137 = call i64 @strlen(ptr @str.const.32)
  %len238 = call i64 @strlen(ptr %alloca9)
  %totallen39 = add i64 %len137, %len238
  %bufsize40 = add i64 %totallen39, 1
  %concat.buf41 = alloca i8, i64 %bufsize40, align 1
  %13 = call ptr @memcpy(ptr %concat.buf41, ptr @str.const.32, i64 %len137)
  %dest242 = getelementptr i8, ptr %concat.buf41, i64 %len137
  %copylen243 = add i64 %len238, 1
  %14 = call ptr @memcpy(ptr %dest242, ptr %alloca9, i64 %copylen243)
  %len144 = call i64 @strlen(ptr %concat.buf41)
  %len245 = call i64 @strlen(ptr @str.const.20)
  %totallen46 = add i64 %len144, %len245
  %bufsize47 = add i64 %totallen46, 1
  %concat.buf48 = alloca i8, i64 %bufsize47, align 1
  %15 = call ptr @memcpy(ptr %concat.buf48, ptr %concat.buf41, i64 %len144)
  %dest249 = getelementptr i8, ptr %concat.buf48, i64 %len144
  %copylen250 = add i64 %len245, 1
  %16 = call ptr @memcpy(ptr %dest249, ptr @str.const.20, i64 %copylen250)
  %len151 = call i64 @strlen(ptr %concat.buf48)
  %len252 = call i64 @strlen(ptr %alloca8)
  %totallen53 = add i64 %len151, %len252
  %bufsize54 = add i64 %totallen53, 1
  %concat.buf55 = alloca i8, i64 %bufsize54, align 1
  %17 = call ptr @memcpy(ptr %concat.buf55, ptr %concat.buf48, i64 %len151)
  %dest256 = getelementptr i8, ptr %concat.buf55, i64 %len151
  %copylen257 = add i64 %len252, 1
  %18 = call ptr @memcpy(ptr %dest256, ptr %alloca8, i64 %copylen257)
  %len158 = call i64 @strlen(ptr %concat.buf55)
  %len259 = call i64 @strlen(ptr @str.const.20)
  %totallen60 = add i64 %len158, %len259
  %bufsize61 = add i64 %totallen60, 1
  %concat.buf62 = alloca i8, i64 %bufsize61, align 1
  %19 = call ptr @memcpy(ptr %concat.buf62, ptr %concat.buf55, i64 %len158)
  %dest263 = getelementptr i8, ptr %concat.buf62, i64 %len158
  %copylen264 = add i64 %len259, 1
  %20 = call ptr @memcpy(ptr %dest263, ptr @str.const.20, i64 %copylen264)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  br label %merge_79

merge_79:                                         ; preds = %then_78, %merge_77
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load65" = load i64, ptr %alloca4, align 8
  %cmpgttmp66 = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load65", 2
  br i1 %cmpgttmp66, label %then_80, label %merge_81

then_80:                                          ; preds = %merge_79
  %len167 = call i64 @strlen(ptr @str.const.32)
  %len268 = call i64 @strlen(ptr %alloca3)
  %totallen69 = add i64 %len167, %len268
  %bufsize70 = add i64 %totallen69, 1
  %concat.buf71 = alloca i8, i64 %bufsize70, align 1
  %22 = call ptr @memcpy(ptr %concat.buf71, ptr @str.const.32, i64 %len167)
  %dest272 = getelementptr i8, ptr %concat.buf71, i64 %len167
  %copylen273 = add i64 %len268, 1
  %23 = call ptr @memcpy(ptr %dest272, ptr %alloca3, i64 %copylen273)
  %len174 = call i64 @strlen(ptr %concat.buf71)
  %len275 = call i64 @strlen(ptr @str.const.20)
  %totallen76 = add i64 %len174, %len275
  %bufsize77 = add i64 %totallen76, 1
  %concat.buf78 = alloca i8, i64 %bufsize77, align 1
  %24 = call ptr @memcpy(ptr %concat.buf78, ptr %concat.buf71, i64 %len174)
  %dest279 = getelementptr i8, ptr %concat.buf78, i64 %len174
  %copylen280 = add i64 %len275, 1
  %25 = call ptr @memcpy(ptr %dest279, ptr @str.const.20, i64 %copylen280)
  %len181 = call i64 @strlen(ptr %concat.buf78)
  %len282 = call i64 @strlen(ptr %alloca)
  %totallen83 = add i64 %len181, %len282
  %bufsize84 = add i64 %totallen83, 1
  %concat.buf85 = alloca i8, i64 %bufsize84, align 1
  %26 = call ptr @memcpy(ptr %concat.buf85, ptr %concat.buf78, i64 %len181)
  %dest286 = getelementptr i8, ptr %concat.buf85, i64 %len181
  %copylen287 = add i64 %len282, 1
  %27 = call ptr @memcpy(ptr %dest286, ptr %alloca, i64 %copylen287)
  %len188 = call i64 @strlen(ptr %concat.buf85)
  %len289 = call i64 @strlen(ptr @str.const.20)
  %totallen90 = add i64 %len188, %len289
  %bufsize91 = add i64 %totallen90, 1
  %concat.buf92 = alloca i8, i64 %bufsize91, align 1
  %28 = call ptr @memcpy(ptr %concat.buf92, ptr %concat.buf85, i64 %len188)
  %dest293 = getelementptr i8, ptr %concat.buf92, i64 %len188
  %copylen294 = add i64 %len289, 1
  %29 = call ptr @memcpy(ptr %dest293, ptr @str.const.20, i64 %copylen294)
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  br label %merge_81

merge_81:                                         ; preds = %then_80, %merge_79
  ret void
}

define i64 @"\D8\AC\D8\AF\D9\88\D9\84_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.\D9\85\D8\AC\D9\85\D9\88\D8\B9_\D9\85\D9\8A\D8\B2\D8\A7\D9\86\D9\8A\D8\A7\D8\AA"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca2 = alloca i64, align 8
  %alloca3 = alloca ptr, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca ptr, align 8
  %alloca7 = alloca ptr, align 8
  %alloca8 = alloca ptr, align 8
  %alloca9 = alloca ptr, align 8
  %alloca10 = alloca i64, align 8
  %alloca11 = alloca i64, align 8
  %alloca12 = alloca i64, align 8
  %alloca13 = alloca i64, align 8
  %alloca14 = alloca i64, align 8
  store i64 0, ptr %alloca14, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load" = load i64, ptr %alloca4, align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load", 0
  br i1 %cmpgttmp, label %then_82, label %merge_83

then_82:                                          ; preds = %entry
  %"%\D9\85.load" = load i64, ptr %alloca14, align 8
  %"%\D9\85\D9\8A\D8\B2\D8\A7\D9\86\D9\8A\D8\A91.load" = load i64, ptr %alloca2, align 8
  %addtmp = add i64 %"%\D9\85.load", %"%\D9\85\D9\8A\D8\B2\D8\A7\D9\86\D9\8A\D8\A91.load"
  store i64 %addtmp, ptr %alloca14, align 8
  br label %merge_83

merge_83:                                         ; preds = %then_82, %entry
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load15" = load i64, ptr %alloca4, align 8
  %cmpgttmp16 = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load15", 1
  br i1 %cmpgttmp16, label %then_84, label %merge_85

then_84:                                          ; preds = %merge_83
  %"%\D9\85.load17" = load i64, ptr %alloca14, align 8
  %"%\D9\85\D9\8A\D8\B2\D8\A7\D9\86\D9\8A\D8\A92.load" = load i64, ptr %alloca10, align 8
  %addtmp18 = add i64 %"%\D9\85.load17", %"%\D9\85\D9\8A\D8\B2\D8\A7\D9\86\D9\8A\D8\A92.load"
  store i64 %addtmp18, ptr %alloca14, align 8
  br label %merge_85

merge_85:                                         ; preds = %then_84, %merge_83
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load19" = load i64, ptr %alloca4, align 8
  %cmpgttmp20 = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load19", 2
  br i1 %cmpgttmp20, label %then_86, label %merge_87

then_86:                                          ; preds = %merge_85
  %"%\D9\85.load21" = load i64, ptr %alloca14, align 8
  %"%\D9\85\D9\8A\D8\B2\D8\A7\D9\86\D9\8A\D8\A93.load" = load i64, ptr %alloca5, align 8
  %addtmp22 = add i64 %"%\D9\85.load21", %"%\D9\85\D9\8A\D8\B2\D8\A7\D9\86\D9\8A\D8\A93.load"
  store i64 %addtmp22, ptr %alloca14, align 8
  br label %merge_87

merge_87:                                         ; preds = %then_86, %merge_85
  %loadtmp = load i64, ptr %alloca14, align 8
  ret i64 %loadtmp
}

define i64 @"\D8\AC\D8\AF\D9\88\D9\84_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.\D9\85\D8\AA\D9\88\D8\B3\D8\B7_\D8\AA\D9\82\D8\AF\D9\85"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca2 = alloca i64, align 8
  %alloca3 = alloca ptr, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca ptr, align 8
  %alloca7 = alloca ptr, align 8
  %alloca8 = alloca ptr, align 8
  %alloca9 = alloca ptr, align 8
  %alloca10 = alloca i64, align 8
  %alloca11 = alloca i64, align 8
  %alloca12 = alloca i64, align 8
  %alloca13 = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load" = load i64, ptr %alloca4, align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load", 0
  br i1 %cmpeqtmp, label %then_88, label %merge_89

then_88:                                          ; preds = %entry
  ret i64 0

merge_89:                                         ; preds = %entry
  %alloca14 = alloca i64, align 8
  store i64 0, ptr %alloca14, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load15" = load i64, ptr %alloca4, align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load15", 0
  br i1 %cmpgttmp, label %then_90, label %merge_91

then_90:                                          ; preds = %merge_89
  %"%\D9\85.load" = load i64, ptr %alloca14, align 8
  %"%\D8\AA\D9\82\D8\AF\D9\851.load" = load i64, ptr %alloca11, align 8
  %addtmp = add i64 %"%\D9\85.load", %"%\D8\AA\D9\82\D8\AF\D9\851.load"
  store i64 %addtmp, ptr %alloca14, align 8
  br label %merge_91

merge_91:                                         ; preds = %then_90, %merge_89
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load16" = load i64, ptr %alloca4, align 8
  %cmpgttmp17 = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load16", 1
  br i1 %cmpgttmp17, label %then_92, label %merge_93

then_92:                                          ; preds = %merge_91
  %"%\D9\85.load18" = load i64, ptr %alloca14, align 8
  %"%\D8\AA\D9\82\D8\AF\D9\852.load" = load i64, ptr %alloca12, align 8
  %addtmp19 = add i64 %"%\D9\85.load18", %"%\D8\AA\D9\82\D8\AF\D9\852.load"
  store i64 %addtmp19, ptr %alloca14, align 8
  br label %merge_93

merge_93:                                         ; preds = %then_92, %merge_91
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load20" = load i64, ptr %alloca4, align 8
  %cmpgttmp21 = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load20", 2
  br i1 %cmpgttmp21, label %then_94, label %merge_95

then_94:                                          ; preds = %merge_93
  %"%\D9\85.load22" = load i64, ptr %alloca14, align 8
  %"%\D8\AA\D9\82\D8\AF\D9\853.load" = load i64, ptr %alloca13, align 8
  %addtmp23 = add i64 %"%\D9\85.load22", %"%\D8\AA\D9\82\D8\AF\D9\853.load"
  store i64 %addtmp23, ptr %alloca14, align 8
  br label %merge_95

merge_95:                                         ; preds = %then_94, %merge_93
  %"%\D9\85.load24" = load i64, ptr %alloca14, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load25" = load i64, ptr %alloca4, align 8
  %divtmp = sdiv i64 %"%\D9\85.load24", %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load25"
  ret i64 %divtmp
}

define i64 @"\D8\A7\D8\B9\D8\B1\D8\B6_\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\86\D9\87\D8\A7\D8\A6\D9\8A"(i64 %"\D9\85\D8\AC\D9\85\D9\88\D8\B9_\D8\B1", i64 %"\D9\85\D8\AA\D9\88\D8\B3\D8\B7_\D8\B1", i64 %"\D8\A3\D8\B9\D9\84\D9\89_\D8\B1", i64 %"\D8\A3\D8\AF\D9\86\D9\89_\D8\B1", i64 %"\D8\B9\D8\AF\D8\AF_\D9\85", i64 %"\D9\85\D8\AC\D9\85\D9\88\D8\B9_\D9\85\D9\8A\D8\B2", i64 %"\D9\85\D8\AA\D9\88\D8\B3\D8\B7_\D8\AA\D9\82") {
entry:
  %"\D9\85\D8\AA\D9\88\D8\B3\D8\B7_\D8\AA\D9\827" = alloca i64, align 8
  store i64 %"\D9\85\D8\AA\D9\88\D8\B3\D8\B7_\D8\AA\D9\82", ptr %"\D9\85\D8\AA\D9\88\D8\B3\D8\B7_\D8\AA\D9\827", align 8
  %"\D9\85\D8\AC\D9\85\D9\88\D8\B9_\D9\85\D9\8A\D8\B26" = alloca i64, align 8
  store i64 %"\D9\85\D8\AC\D9\85\D9\88\D8\B9_\D9\85\D9\8A\D8\B2", ptr %"\D9\85\D8\AC\D9\85\D9\88\D8\B9_\D9\85\D9\8A\D8\B26", align 8
  %"\D8\B9\D8\AF\D8\AF_\D9\855" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF_\D9\85", ptr %"\D8\B9\D8\AF\D8\AF_\D9\855", align 8
  %"\D8\A3\D8\AF\D9\86\D9\89_\D8\B14" = alloca i64, align 8
  store i64 %"\D8\A3\D8\AF\D9\86\D9\89_\D8\B1", ptr %"\D8\A3\D8\AF\D9\86\D9\89_\D8\B14", align 8
  %"\D8\A3\D8\B9\D9\84\D9\89_\D8\B13" = alloca i64, align 8
  store i64 %"\D8\A3\D8\B9\D9\84\D9\89_\D8\B1", ptr %"\D8\A3\D8\B9\D9\84\D9\89_\D8\B13", align 8
  %"\D9\85\D8\AA\D9\88\D8\B3\D8\B7_\D8\B12" = alloca i64, align 8
  store i64 %"\D9\85\D8\AA\D9\88\D8\B3\D8\B7_\D8\B1", ptr %"\D9\85\D8\AA\D9\88\D8\B3\D8\B7_\D8\B12", align 8
  %"\D9\85\D8\AC\D9\85\D9\88\D8\B9_\D8\B11" = alloca i64, align 8
  store i64 %"\D9\85\D8\AC\D9\85\D9\88\D8\B9_\D8\B1", ptr %"\D9\85\D8\AC\D9\85\D9\88\D8\B9_\D8\B11", align 8
  %calltmp = call i64 @"\D8\A7\D8\B9\D8\B1\D8\B6_\D8\B9\D9\86\D9\88\D8\A7\D9\86"(ptr @str.const.124)
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.125)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.129)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.145)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.149)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.163)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.167)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %"%\D9\85\D8\AA\D9\88\D8\B3\D8\B7_\D8\AA\D9\82.load" = load i64, ptr %"\D9\85\D8\AA\D9\88\D8\B3\D8\B7_\D8\AA\D9\827", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\85\D8\AA\D9\88\D8\B3\D8\B7_\D8\AA\D9\82.load", 70
  br i1 %cmpgttmp, label %then_96, label %else_98

then_96:                                          ; preds = %entry
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.171)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  br label %merge_97

else_98:                                          ; preds = %entry
  %"%\D9\85\D8\AA\D9\88\D8\B3\D8\B7_\D8\AA\D9\82.load8" = load i64, ptr %"\D9\85\D8\AA\D9\88\D8\B3\D8\B7_\D8\AA\D9\827", align 8
  %cmpgttmp9 = icmp sgt i64 %"%\D9\85\D8\AA\D9\88\D8\B3\D8\B7_\D8\AA\D9\82.load8", 40
  br i1 %cmpgttmp9, label %then_99, label %else_101

merge_97:                                         ; preds = %merge_100, %then_96
  %"%\D8\A3\D8\B9\D9\84\D9\89_\D8\B1.load" = load i64, ptr %"\D8\A3\D8\B9\D9\84\D9\89_\D8\B13", align 8
  %"%\D8\A3\D8\AF\D9\86\D9\89_\D8\B1.load" = load i64, ptr %"\D8\A3\D8\AF\D9\86\D9\89_\D8\B14", align 8
  %subtmp = sub i64 %"%\D8\A3\D8\B9\D9\84\D9\89_\D8\B1.load", %"%\D8\A3\D8\AF\D9\86\D9\89_\D8\B1.load"
  %cmpgttmp10 = icmp sgt i64 %subtmp, 5000
  br i1 %cmpgttmp10, label %then_102, label %else_104

then_99:                                          ; preds = %else_98
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.175)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  br label %merge_100

else_101:                                         ; preds = %else_98
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.179)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  br label %merge_100

merge_100:                                        ; preds = %else_101, %then_99
  br label %merge_97

then_102:                                         ; preds = %merge_97
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.183)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  br label %merge_103

else_104:                                         ; preds = %merge_97
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.187)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  br label %merge_103

merge_103:                                        ; preds = %else_104, %then_102
  ret i64 0
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %calltmp = call i64 @"\D8\A7\D8\B9\D8\B1\D8\B6_\D8\B9\D9\86\D9\88\D8\A7\D9\86"(ptr @str.const.191)
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.192)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.196)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %alloca = alloca i64, align 8
  %alloca1 = alloca i64, align 8
  %"%350.load" = load i64, ptr %alloca, align 8
  store i64 %"%350.load", ptr %alloca1, align 8
  %"%\D9\85\D9\88\D8\B8\D9\81\D9\88\D9\86.load" = load i64, ptr %alloca1, align 8
  call void @"\D8\AC\D8\AF\D9\88\D9\84_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.\D8\A3\D8\B6\D9\81"(i64 %"%\D9\85\D9\88\D8\B8\D9\81\D9\88\D9\86.load", ptr @str.const.200, i64 8000, ptr @str.const.201)
  %"%\D9\85\D9\88\D8\B8\D9\81\D9\88\D9\86.load2" = load i64, ptr %alloca1, align 8
  call void @"\D8\AC\D8\AF\D9\88\D9\84_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.\D8\A3\D8\B6\D9\81"(i64 %"%\D9\85\D9\88\D8\B8\D9\81\D9\88\D9\86.load2", ptr @str.const.202, i64 9500, ptr @str.const.203)
  %"%\D9\85\D9\88\D8\B8\D9\81\D9\88\D9\86.load3" = load i64, ptr %alloca1, align 8
  call void @"\D8\AC\D8\AF\D9\88\D9\84_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.\D8\A3\D8\B6\D9\81"(i64 %"%\D9\85\D9\88\D8\B8\D9\81\D9\88\D9\86.load3", ptr @str.const.204, i64 7000, ptr @str.const.205)
  %"%\D9\85\D9\88\D8\B8\D9\81\D9\88\D9\86.load4" = load i64, ptr %alloca1, align 8
  call void @"\D8\AC\D8\AF\D9\88\D9\84_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.\D8\A3\D8\B6\D9\81"(i64 %"%\D9\85\D9\88\D8\B8\D9\81\D9\88\D9\86.load4", ptr @str.const.206, i64 12000, ptr @str.const.201)
  %"%\D9\85\D9\88\D8\B8\D9\81\D9\88\D9\86.load5" = load i64, ptr %alloca1, align 8
  call void @"\D8\AC\D8\AF\D9\88\D9\84_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.\D8\A3\D8\B6\D9\81"(i64 %"%\D9\85\D9\88\D8\B8\D9\81\D9\88\D9\86.load5", ptr @str.const.208, i64 6500, ptr @str.const.209)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.210)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.214)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %"%\D9\85\D9\88\D8\B8\D9\81\D9\88\D9\86.load6" = load i64, ptr %alloca1, align 8
  call void @"\D8\AC\D8\AF\D9\88\D9\84_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.\D8\A7\D8\B9\D8\B1\D8\B6_\D8\AC\D8\AF\D9\88\D9\84"(i64 %"%\D9\85\D9\88\D8\B8\D9\81\D9\88\D9\86.load6")
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.218)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.222)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %"%\D9\85\D9\88\D8\B8\D9\81\D9\88\D9\86.load7" = load i64, ptr %alloca1, align 8
  call void @"\D8\AC\D8\AF\D9\88\D9\84_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.\D8\A7\D8\B9\D8\B1\D8\B6_\D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA"(i64 %"%\D9\85\D9\88\D8\B8\D9\81\D9\88\D9\86.load7")
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.226)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.230)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %alloca8 = alloca i64, align 8
  %alloca9 = alloca i64, align 8
  %"%379.load" = load i64, ptr %alloca8, align 8
  store i64 %"%379.load", ptr %alloca9, align 8
  %"%\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load" = load i64, ptr %alloca9, align 8
  call void @"\D8\AC\D8\AF\D9\88\D9\84_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.\D8\A3\D8\B6\D9\81_\D9\85\D8\B4\D8\B1\D9\88\D8\B9"(i64 %"%\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load", ptr @str.const.234, ptr @str.const.235, i64 50000, i64 75)
  %"%\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load10" = load i64, ptr %alloca9, align 8
  call void @"\D8\AC\D8\AF\D9\88\D9\84_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.\D8\A3\D8\B6\D9\81_\D9\85\D8\B4\D8\B1\D9\88\D8\B9"(i64 %"%\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load10", ptr @str.const.236, ptr @str.const.237, i64 30000, i64 45)
  %"%\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load11" = load i64, ptr %alloca9, align 8
  call void @"\D8\AC\D8\AF\D9\88\D9\84_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.\D8\A3\D8\B6\D9\81_\D9\85\D8\B4\D8\B1\D9\88\D8\B9"(i64 %"%\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load11", ptr @str.const.238, ptr @str.const.239, i64 20000, i64 10)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.240)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.244)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %"%\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load12" = load i64, ptr %alloca9, align 8
  call void @"\D8\AC\D8\AF\D9\88\D9\84_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.\D8\A7\D8\B9\D8\B1\D8\B6_\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9"(i64 %"%\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9.load12")
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.248)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %"%\D9\85\D9\88\D8\B8\D9\81\D9\88\D9\86.load13" = load i64, ptr %alloca1, align 8
  %calltmp14 = call i64 @"\D8\AC\D8\AF\D9\88\D9\84_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.\D9\85\D8\AC\D9\85\D9\88\D8\B9_\D8\B1\D9\88\D8\A7\D8\AA\D8\A8"(i64 %"%\D9\85\D9\88\D8\B8\D9\81\D9\88\D9\86.load13")
  %"%\D9\85\D9\88\D8\B8\D9\81\D9\88\D9\86.load15" = load i64, ptr %alloca1, align 8
  %calltmp16 = call i64 @"\D8\AC\D8\AF\D9\88\D9\84_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.\D9\85\D8\AA\D9\88\D8\B3\D8\B7_\D8\B1\D9\88\D8\A7\D8\AA\D8\A8"(i64 %"%\D9\85\D9\88\D8\B8\D9\81\D9\88\D9\86.load15")
  %"%\D9\85\D9\88\D8\B8\D9\81\D9\88\D9\86.load17" = load i64, ptr %alloca1, align 8
  %calltmp18 = call i64 @"\D8\AC\D8\AF\D9\88\D9\84_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.\D8\A3\D8\B9\D9\84\D9\89_\D8\B1\D8\A7\D8\AA\D8\A8"(i64 %"%\D9\85\D9\88\D8\B8\D9\81\D9\88\D9\86.load17")
  %"%\D9\85\D9\88\D8\B8\D9\81\D9\88\D9\86.load19" = load i64, ptr %alloca1, align 8
  %calltmp20 = call i64 @"\D8\AC\D8\AF\D9\88\D9\84_\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86.\D8\A3\D8\AF\D9\86\D9\89_\D8\B1\D8\A7\D8\AA\D8\A8"(i64 %"%\D9\85\D9\88\D8\B8\D9\81\D9\88\D9\86.load19")
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.252)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.256)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  ret i64 0
}

declare i32 @printf(ptr, ...)

declare i64 @strlen(ptr)

declare ptr @memcpy(ptr, ptr, i64)

declare void @"\D9\85\D8\AC\D9\85\D9\88\D8\B9_\D8\B1\D9\88\D8\A7\D8\AA\D8\A8"()

declare void @"\D9\85\D8\AA\D9\88\D8\B3\D8\B7_\D8\B1\D9\88\D8\A7\D8\AA\D8\A8"()

declare void @"\D8\A3\D8\B9\D9\84\D9\89_\D8\B1\D8\A7\D8\AA\D8\A8"()

declare void @"\D8\A3\D8\AF\D9\86\D9\89_\D8\B1\D8\A7\D8\AA\D8\A8"()

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
