; ModuleID = '04_medium_design_patterns'
source_filename = "04_medium_design_patterns"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [4 x i8] c"  [\00", align 1
@str.const.1 = private unnamed_addr constant [13 x i8] c"] \D8\AA\D9\84\D9\82\D9\89: \00", align 1
@str.const.2 = private unnamed_addr constant [5 x i8] c" -> \00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.3 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.5 = private unnamed_addr constant [15 x i8] c"  \D8\AA\D8\B4\D8\BA\D9\8A\D9\84: \00", align 1
@str.const.6 = private unnamed_addr constant [4 x i8] c" - \00", align 1
@str.const.10 = private unnamed_addr constant [15 x i8] c"  \D8\AA\D8\B1\D8\A7\D8\AC\D8\B9: \00", align 1
@str.const.14 = private unnamed_addr constant [33 x i8] c"  \D9\84\D8\A7 \D9\8A\D9\85\D9\83\D9\86 \D8\A7\D9\84\D8\AA\D8\B1\D8\A7\D8\AC\D8\B9: \00", align 1
@str.const.15 = private unnamed_addr constant [17 x i8] c" \D8\BA\D9\8A\D8\B1 \D9\85\D8\B4\D8\BA\D9\84\00", align 1
@str.const.19 = private unnamed_addr constant [17 x i8] c"  \D8\A7\D9\86\D8\AA\D9\82\D8\A7\D9\84: \00", align 1
@0 = private unnamed_addr constant [7 x i8] c"\D8\AC\D9\85\D8\B9\00", align 1
@1 = private unnamed_addr constant [7 x i8] c"\D8\B6\D8\B1\D8\A8\00", align 1
@2 = private unnamed_addr constant [9 x i8] c"\D8\A3\D9\83\D8\A8\D8\B1\00", align 1
@str.const.24 = private unnamed_addr constant [41 x i8] c"\D8\A7\D8\B3\D8\AA\D8\B1\D8\A7\D8\AA\D9\8A\D8\AC\D9\8A\D8\A9 \D8\BA\D9\8A\D8\B1 \D9\85\D8\B9\D8\B1\D9\88\D9\81\D8\A9\00", align 1
@str.const.28 = private unnamed_addr constant [11 x i8] c"\D9\85\D8\AC\D9\87\D9\88\D9\84\00", align 1
@str.const.29 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.30 = private unnamed_addr constant [27 x i8] c"==========================\00", align 1
@str.const.34 = private unnamed_addr constant [13 x i8] c"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9: \00", align 1
@str.const.38 = private unnamed_addr constant [7 x i8] c"\D9\85\D9\86: \00", align 1
@str.const.42 = private unnamed_addr constant [15 x i8] c"\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9: \00", align 1
@str.const.45 = private unnamed_addr constant [13 x i8] c"\D9\85\D8\AD\D8\AA\D9\88\D9\89: \00", align 1
@str.const.53 = private unnamed_addr constant [45 x i8] c"=== \D9\85\D8\A7\D9\83\D8\B1\D9\88 \D8\A3\D9\86\D9\85\D8\A7\D8\B7 \D8\A7\D9\84\D8\AA\D8\B5\D9\85\D9\8A\D9\85 ===\00", align 1
@str.const.57 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.61 = private unnamed_addr constant [30 x i8] c"--- \D9\86\D9\85\D8\B7 \D8\A7\D9\84\D9\85\D8\B1\D8\A7\D9\82\D8\A8 ---\00", align 1
@str.const.65 = private unnamed_addr constant [20 x i8] c"\D8\B3\D8\AC\D9\84_\D8\A7\D9\84\D9\86\D8\B8\D8\A7\D9\85\00", align 1
@str.const.66 = private unnamed_addr constant [22 x i8] c"\D9\84\D9\88\D8\AD\D8\A9_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85\00", align 1
@str.const.67 = private unnamed_addr constant [20 x i8] c"\D8\AA\D8\B3\D8\AC\D9\8A\D9\84_\D8\AF\D8\AE\D9\88\D9\84\00", align 1
@str.const.68 = private unnamed_addr constant [26 x i8] c"\D8\A7\D9\84\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85 \D8\A3\D8\AD\D9\85\D8\AF\00", align 1
@str.const.71 = private unnamed_addr constant [20 x i8] c"\D9\85\D8\B4\D9\83\D9\84\D8\A9_\D9\86\D8\B8\D8\A7\D9\85\00", align 1
@str.const.72 = private unnamed_addr constant [22 x i8] c"\D9\81\D8\B4\D9\84 \D8\A7\D9\84\D8\A7\D8\AA\D8\B5\D8\A7\D9\84\00", align 1
@str.const.75 = private unnamed_addr constant [16 x i8] c"\D8\A5\D8\B4\D8\B9\D8\A7\D8\B1\D8\A7\D8\AA \00", align 1
@str.const.76 = private unnamed_addr constant [3 x i8] c": \00", align 1
@str.const.83 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.87 = private unnamed_addr constant [26 x i8] c"--- \D9\86\D9\85\D8\B7 \D8\A7\D9\84\D8\A3\D9\85\D8\B1 ---\00", align 1
@str.const.91 = private unnamed_addr constant [7 x i8] c"\D8\AD\D9\81\D8\B8\00", align 1
@str.const.92 = private unnamed_addr constant [18 x i8] c"\D8\AD\D9\81\D8\B8 \D8\A7\D9\84\D9\85\D9\84\D9\81\00", align 1
@str.const.93 = private unnamed_addr constant [7 x i8] c"\D9\86\D8\B3\D8\AE\00", align 1
@str.const.94 = private unnamed_addr constant [16 x i8] c"\D9\86\D8\B3\D8\AE \D8\A7\D9\84\D9\86\D8\B5\00", align 1
@str.const.95 = private unnamed_addr constant [7 x i8] c"\D8\AD\D8\B0\D9\81\00", align 1
@str.const.96 = private unnamed_addr constant [18 x i8] c"\D8\AD\D8\B0\D9\81 \D8\A7\D9\84\D8\B3\D8\B7\D8\B1\00", align 1
@str.const.97 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.101 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.105 = private unnamed_addr constant [28 x i8] c"--- \D9\86\D9\85\D8\B7 \D8\A7\D9\84\D8\AD\D8\A7\D9\84\D8\A9 ---\00", align 1
@str.const.109 = private unnamed_addr constant [9 x i8] c"\D8\AE\D8\A7\D9\85\D9\84\00", align 1
@str.const.110 = private unnamed_addr constant [24 x i8] c"\D8\AC\D8\A7\D8\B1\D9\8A_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84\00", align 1
@str.const.111 = private unnamed_addr constant [9 x i8] c"\D8\AC\D8\A7\D9\87\D8\B2\00", align 1
@str.const.112 = private unnamed_addr constant [24 x i8] c"\D9\82\D9\8A\D8\AF_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9\00", align 1
@str.const.113 = private unnamed_addr constant [11 x i8] c"\D9\85\D9\83\D8\AA\D9\85\D9\84\00", align 1
@str.const.114 = private unnamed_addr constant [32 x i8] c"\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D8\A9 \D8\A7\D9\84\D9\86\D9\87\D8\A7\D8\A6\D9\8A\D8\A9: \00", align 1
@str.const.118 = private unnamed_addr constant [30 x i8] c"\D8\B9\D8\AF\D8\AF \D8\A7\D9\84\D8\A7\D9\86\D8\AA\D9\82\D8\A7\D9\84\D8\A7\D8\AA: \00", align 1
@str.const.121 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.125 = private unnamed_addr constant [40 x i8] c"--- \D9\86\D9\85\D8\B7 \D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D8\B1\D8\A7\D8\AA\D9\8A\D8\AC\D9\8A\D8\A9 ---\00", align 1
@str.const.130 = private unnamed_addr constant [19 x i8] c"\D8\AC\D9\85\D8\B9(10\D8\8C 20) = \00", align 1
@str.const.134 = private unnamed_addr constant [19 x i8] c"\D8\B6\D8\B1\D8\A8(10\D8\8C 20) = \00", align 1
@str.const.138 = private unnamed_addr constant [21 x i8] c"\D8\A3\D9\83\D8\A8\D8\B1(10\D8\8C 20) = \00", align 1
@str.const.141 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.145 = private unnamed_addr constant [28 x i8] c"--- \D9\86\D9\85\D8\B7 \D8\A7\D9\84\D8\A8\D9\86\D8\A7\D8\A1 ---\00", align 1
@str.const.149 = private unnamed_addr constant [26 x i8] c"\D8\A7\D8\AC\D8\AA\D9\85\D8\A7\D8\B9 \D8\A7\D9\84\D9\81\D8\B1\D9\8A\D9\82\00", align 1
@str.const.150 = private unnamed_addr constant [24 x i8] c"\D9\85\D8\AF\D9\8A\D8\B1 \D8\A7\D9\84\D9\85\D8\B4\D8\B1\D9\88\D8\B9\00", align 1
@str.const.151 = private unnamed_addr constant [47 x i8] c"\D8\A7\D8\AC\D8\AA\D9\85\D8\A7\D8\B9 \D8\A7\D9\84\D8\B3\D8\A7\D8\B9\D8\A9 10 \D8\B5\D8\A8\D8\A7\D8\AD\D8\A7 \D8\BA\D8\AF\D8\A7\00", align 1
@str.const.152 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.156 = private unnamed_addr constant [20 x i8] c"\D8\AA\D8\AD\D8\AF\D9\8A\D8\AB \D8\B9\D8\A7\D8\AC\D9\84\00", align 1
@str.const.157 = private unnamed_addr constant [22 x i8] c"\D9\81\D8\B1\D9\8A\D9\82 \D8\A7\D9\84\D8\A3\D9\85\D8\A7\D9\86\00", align 1
@str.const.158 = private unnamed_addr constant [51 x i8] c"\D9\8A\D8\B1\D8\AC\D9\89 \D8\AA\D8\AD\D8\AF\D9\8A\D8\AB \D9\83\D9\84\D9\85\D8\A9 \D8\A7\D9\84\D9\85\D8\B1\D9\88\D8\B1 \D9\81\D9\88\D8\B1\D8\A7\00", align 1
@str.const.159 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.163 = private unnamed_addr constant [45 x i8] c"=== \D8\A7\D9\86\D8\AA\D9\87\D8\AA \D8\A3\D9\86\D9\85\D8\A7\D8\B7 \D8\A7\D9\84\D8\AA\D8\B5\D9\85\D9\8A\D9\85 ===\00", align 1

define void @"\D9\85\D8\B1\D8\A7\D9\82\D8\A8.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85") {
entry:
  %"\D8\A7\D9\84\D8\A7\D8\B3\D9\852" = alloca ptr, align 8
  store ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85", ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\852", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca3 = alloca i64, align 8
  store ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\852", ptr %alloca, align 8
  store i64 0, ptr %alloca3, align 8
  ret void
}

define void @"\D9\85\D8\B1\D8\A7\D9\82\D8\A8.\D8\B9\D8\A7\D9\84\D8\AC"(i64 %self, ptr %"\D8\AD\D8\AF\D8\AB_\D8\A7\D8\B3\D9\85", ptr %"\D8\AD\D8\AF\D8\AB_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA") {
entry:
  %"\D8\AD\D8\AF\D8\AB_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA3" = alloca ptr, align 8
  store ptr %"\D8\AD\D8\AF\D8\AB_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", ptr %"\D8\AD\D8\AF\D8\AB_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA3", align 8
  %"\D8\AD\D8\AF\D8\AB_\D8\A7\D8\B3\D9\852" = alloca ptr, align 8
  store ptr %"\D8\AD\D8\AF\D8\AB_\D8\A7\D8\B3\D9\85", ptr %"\D8\AD\D8\AF\D8\AB_\D8\A7\D8\B3\D9\852", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca4 = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A5\D8\B4\D8\B9\D8\A7\D8\B1\D8\A7\D8\AA.load" = load i64, ptr %alloca4, align 8
  %addtmp = add i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A5\D8\B4\D8\B9\D8\A7\D8\B1\D8\A7\D8\AA.load", 1
  store i64 %addtmp, ptr %alloca4, align 8
  %len1 = call i64 @strlen(ptr @str.const)
  %len2 = call i64 @strlen(ptr %alloca)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %alloca, i64 %copylen2)
  %len15 = call i64 @strlen(ptr %concat.buf)
  %len26 = call i64 @strlen(ptr @str.const.1)
  %totallen7 = add i64 %len15, %len26
  %bufsize8 = add i64 %totallen7, 1
  %concat.buf9 = alloca i8, i64 %bufsize8, align 1
  %2 = call ptr @memcpy(ptr %concat.buf9, ptr %concat.buf, i64 %len15)
  %dest210 = getelementptr i8, ptr %concat.buf9, i64 %len15
  %copylen211 = add i64 %len26, 1
  %3 = call ptr @memcpy(ptr %dest210, ptr @str.const.1, i64 %copylen211)
  %len112 = call i64 @strlen(ptr %concat.buf9)
  %len213 = call i64 @strlen(ptr %"\D8\AD\D8\AF\D8\AB_\D8\A7\D8\B3\D9\852")
  %totallen14 = add i64 %len112, %len213
  %bufsize15 = add i64 %totallen14, 1
  %concat.buf16 = alloca i8, i64 %bufsize15, align 1
  %4 = call ptr @memcpy(ptr %concat.buf16, ptr %concat.buf9, i64 %len112)
  %dest217 = getelementptr i8, ptr %concat.buf16, i64 %len112
  %copylen218 = add i64 %len213, 1
  %5 = call ptr @memcpy(ptr %dest217, ptr %"\D8\AD\D8\AF\D8\AB_\D8\A7\D8\B3\D9\852", i64 %copylen218)
  %len119 = call i64 @strlen(ptr %concat.buf16)
  %len220 = call i64 @strlen(ptr @str.const.2)
  %totallen21 = add i64 %len119, %len220
  %bufsize22 = add i64 %totallen21, 1
  %concat.buf23 = alloca i8, i64 %bufsize22, align 1
  %6 = call ptr @memcpy(ptr %concat.buf23, ptr %concat.buf16, i64 %len119)
  %dest224 = getelementptr i8, ptr %concat.buf23, i64 %len119
  %copylen225 = add i64 %len220, 1
  %7 = call ptr @memcpy(ptr %dest224, ptr @str.const.2, i64 %copylen225)
  %len126 = call i64 @strlen(ptr %concat.buf23)
  %len227 = call i64 @strlen(ptr %"\D8\AD\D8\AF\D8\AB_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA3")
  %totallen28 = add i64 %len126, %len227
  %bufsize29 = add i64 %totallen28, 1
  %concat.buf30 = alloca i8, i64 %bufsize29, align 1
  %8 = call ptr @memcpy(ptr %concat.buf30, ptr %concat.buf23, i64 %len126)
  %dest231 = getelementptr i8, ptr %concat.buf30, i64 %len126
  %copylen232 = add i64 %len227, 1
  %9 = call ptr @memcpy(ptr %dest231, ptr %"\D8\AD\D8\AF\D8\AB_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA3", i64 %copylen232)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf30)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  ret void
}

define i64 @"\D9\85\D8\B1\D8\A7\D9\82\D8\A8.\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A7\D8\AA"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca2 = alloca i64, align 8
  %loadtmp = load i64, ptr %alloca2, align 8
  ret i64 %loadtmp
}

define void @"\D8\A3\D9\85\D8\B1.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85", ptr %"\D8\A7\D9\84\D9\88\D8\B5\D9\81") {
entry:
  %"\D8\A7\D9\84\D9\88\D8\B5\D9\813" = alloca ptr, align 8
  store ptr %"\D8\A7\D9\84\D9\88\D8\B5\D9\81", ptr %"\D8\A7\D9\84\D9\88\D8\B5\D9\813", align 8
  %"\D8\A7\D9\84\D8\A7\D8\B3\D9\852" = alloca ptr, align 8
  store ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85", ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\852", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca4 = alloca ptr, align 8
  %alloca5 = alloca i1, align 1
  store ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\852", ptr %alloca, align 8
  store ptr %"\D8\A7\D9\84\D9\88\D8\B5\D9\813", ptr %alloca4, align 8
  store i64 0, ptr %alloca5, align 8
  ret void
}

define void @"\D8\A3\D9\85\D8\B1.\D8\B4\D8\BA\D9\91\D9\84"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca i1, align 1
  %len1 = call i64 @strlen(ptr @str.const.5)
  %len2 = call i64 @strlen(ptr %alloca)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.5, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %alloca, i64 %copylen2)
  %len14 = call i64 @strlen(ptr %concat.buf)
  %len25 = call i64 @strlen(ptr @str.const.6)
  %totallen6 = add i64 %len14, %len25
  %bufsize7 = add i64 %totallen6, 1
  %concat.buf8 = alloca i8, i64 %bufsize7, align 1
  %2 = call ptr @memcpy(ptr %concat.buf8, ptr %concat.buf, i64 %len14)
  %dest29 = getelementptr i8, ptr %concat.buf8, i64 %len14
  %copylen210 = add i64 %len25, 1
  %3 = call ptr @memcpy(ptr %dest29, ptr @str.const.6, i64 %copylen210)
  %len111 = call i64 @strlen(ptr %concat.buf8)
  %len212 = call i64 @strlen(ptr %alloca2)
  %totallen13 = add i64 %len111, %len212
  %bufsize14 = add i64 %totallen13, 1
  %concat.buf15 = alloca i8, i64 %bufsize14, align 1
  %4 = call ptr @memcpy(ptr %concat.buf15, ptr %concat.buf8, i64 %len111)
  %dest216 = getelementptr i8, ptr %concat.buf15, i64 %len111
  %copylen217 = add i64 %len212, 1
  %5 = call ptr @memcpy(ptr %dest216, ptr %alloca2, i64 %copylen217)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf15)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  store i64 0, ptr %alloca3, align 8
  ret void
}

define void @"\D8\A3\D9\85\D8\B1.\D8\AA\D8\B1\D8\A7\D8\AC\D8\B9"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca i1, align 1
  %"%\D8\AA\D9\85_\D8\AA\D8\B4\D8\BA\D9\8A\D9\84\D9\87.load" = load i1, ptr %alloca3, align 1
  br i1 %"%\D8\AA\D9\85_\D8\AA\D8\B4\D8\BA\D9\8A\D9\84\D9\87.load", label %then_0, label %else_2

then_0:                                           ; preds = %entry
  %len1 = call i64 @strlen(ptr @str.const.10)
  %len2 = call i64 @strlen(ptr %alloca)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.10, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %alloca, i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  store i64 0, ptr %alloca3, align 8
  br label %merge_1

else_2:                                           ; preds = %entry
  %len14 = call i64 @strlen(ptr @str.const.14)
  %len25 = call i64 @strlen(ptr %alloca)
  %totallen6 = add i64 %len14, %len25
  %bufsize7 = add i64 %totallen6, 1
  %concat.buf8 = alloca i8, i64 %bufsize7, align 1
  %4 = call ptr @memcpy(ptr %concat.buf8, ptr @str.const.14, i64 %len14)
  %dest29 = getelementptr i8, ptr %concat.buf8, i64 %len14
  %copylen210 = add i64 %len25, 1
  %5 = call ptr @memcpy(ptr %dest29, ptr %alloca, i64 %copylen210)
  %len111 = call i64 @strlen(ptr %concat.buf8)
  %len212 = call i64 @strlen(ptr @str.const.15)
  %totallen13 = add i64 %len111, %len212
  %bufsize14 = add i64 %totallen13, 1
  %concat.buf15 = alloca i8, i64 %bufsize14, align 1
  %6 = call ptr @memcpy(ptr %concat.buf15, ptr %concat.buf8, i64 %len111)
  %dest216 = getelementptr i8, ptr %concat.buf15, i64 %len111
  %copylen217 = add i64 %len212, 1
  %7 = call ptr @memcpy(ptr %dest216, ptr @str.const.15, i64 %copylen217)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf15)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  br label %merge_1

merge_1:                                          ; preds = %else_2, %then_0
  ret void
}

define void @"\D8\A2\D9\84\D8\A9_\D8\AD\D8\A7\D9\84\D8\A7\D8\AA.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, ptr %"\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D8\A8\D8\AA\D8\AF\D8\A7\D8\A6\D9\8A\D8\A9") {
entry:
  %"\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D8\A8\D8\AA\D8\AF\D8\A7\D8\A6\D9\8A\D8\A92" = alloca ptr, align 8
  store ptr %"\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D8\A8\D8\AA\D8\AF\D8\A7\D8\A6\D9\8A\D8\A9", ptr %"\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D8\A8\D8\AA\D8\AF\D8\A7\D8\A6\D9\8A\D8\A92", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca3 = alloca i64, align 8
  store ptr %"\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D8\A8\D8\AA\D8\AF\D8\A7\D8\A6\D9\8A\D8\A92", ptr %alloca, align 8
  store i64 0, ptr %alloca3, align 8
  ret void
}

define void @"\D8\A2\D9\84\D8\A9_\D8\AD\D8\A7\D9\84\D8\A7\D8\AA.\D8\A7\D9\86\D8\AA\D9\82\D9\84"(i64 %self, ptr %"\D8\AD\D8\A7\D9\84\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9") {
entry:
  %"\D8\AD\D8\A7\D9\84\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A92" = alloca ptr, align 8
  store ptr %"\D8\AD\D8\A7\D9\84\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9", ptr %"\D8\AD\D8\A7\D9\84\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A92", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca3 = alloca i64, align 8
  %len1 = call i64 @strlen(ptr @str.const.19)
  %len2 = call i64 @strlen(ptr %alloca)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.19, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %alloca, i64 %copylen2)
  %len14 = call i64 @strlen(ptr %concat.buf)
  %len25 = call i64 @strlen(ptr @str.const.2)
  %totallen6 = add i64 %len14, %len25
  %bufsize7 = add i64 %totallen6, 1
  %concat.buf8 = alloca i8, i64 %bufsize7, align 1
  %2 = call ptr @memcpy(ptr %concat.buf8, ptr %concat.buf, i64 %len14)
  %dest29 = getelementptr i8, ptr %concat.buf8, i64 %len14
  %copylen210 = add i64 %len25, 1
  %3 = call ptr @memcpy(ptr %dest29, ptr @str.const.2, i64 %copylen210)
  %len111 = call i64 @strlen(ptr %concat.buf8)
  %len212 = call i64 @strlen(ptr %"\D8\AD\D8\A7\D9\84\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A92")
  %totallen13 = add i64 %len111, %len212
  %bufsize14 = add i64 %totallen13, 1
  %concat.buf15 = alloca i8, i64 %bufsize14, align 1
  %4 = call ptr @memcpy(ptr %concat.buf15, ptr %concat.buf8, i64 %len111)
  %dest216 = getelementptr i8, ptr %concat.buf15, i64 %len111
  %copylen217 = add i64 %len212, 1
  %5 = call ptr @memcpy(ptr %dest216, ptr %"\D8\AD\D8\A7\D9\84\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A92", i64 %copylen217)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf15)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  store ptr %"\D8\AD\D8\A7\D9\84\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A92", ptr %alloca, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A7\D9\86\D8\AA\D9\82\D8\A7\D9\84\D8\A7\D8\AA.load" = load i64, ptr %alloca3, align 8
  %addtmp = add i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A7\D9\86\D8\AA\D9\82\D8\A7\D9\84\D8\A7\D8\AA.load", 1
  store i64 %addtmp, ptr %alloca3, align 8
  ret void
}

define ptr @"\D8\A2\D9\84\D8\A9_\D8\AD\D8\A7\D9\84\D8\A7\D8\AA.\D8\A7\D9\84\D8\AD\D8\A7\D9\84"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca2 = alloca i64, align 8
  %loadtmp = load ptr, ptr %alloca, align 8
  ret ptr %loadtmp
}

define i64 @"\D8\A2\D9\84\D8\A9_\D8\AD\D8\A7\D9\84\D8\A7\D8\AA.\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\AA\D8\BA\D9\8A\D9\8A\D8\B1\D8\A7\D8\AA"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca2 = alloca i64, align 8
  %loadtmp = load i64, ptr %alloca2, align 8
  ret i64 %loadtmp
}

define i64 @"\D8\A7\D8\B3\D8\AA\D8\B1\D8\A7\D8\AA\D9\8A\D8\AC\D9\8A\D8\A9_\D8\AC\D9\85\D8\B9"(i64 %"\D8\A3", i64 %"\D8\A8") {
entry:
  %"\D8\A82" = alloca i64, align 8
  store i64 %"\D8\A8", ptr %"\D8\A82", align 8
  %"\D8\A31" = alloca i64, align 8
  store i64 %"\D8\A3", ptr %"\D8\A31", align 8
  %"%\D8\A3.load" = load i64, ptr %"\D8\A31", align 8
  %"%\D8\A8.load" = load i64, ptr %"\D8\A82", align 8
  %addtmp = add i64 %"%\D8\A3.load", %"%\D8\A8.load"
  ret i64 %addtmp
}

define i64 @"\D8\A7\D8\B3\D8\AA\D8\B1\D8\A7\D8\AA\D9\8A\D8\AC\D9\8A\D8\A9_\D8\B6\D8\B1\D8\A8"(i64 %"\D8\A3", i64 %"\D8\A8") {
entry:
  %"\D8\A82" = alloca i64, align 8
  store i64 %"\D8\A8", ptr %"\D8\A82", align 8
  %"\D8\A31" = alloca i64, align 8
  store i64 %"\D8\A3", ptr %"\D8\A31", align 8
  %"%\D8\A3.load" = load i64, ptr %"\D8\A31", align 8
  %"%\D8\A8.load" = load i64, ptr %"\D8\A82", align 8
  %multmp = mul i64 %"%\D8\A3.load", %"%\D8\A8.load"
  ret i64 %multmp
}

define i64 @"\D8\A7\D8\B3\D8\AA\D8\B1\D8\A7\D8\AA\D9\8A\D8\AC\D9\8A\D8\A9_\D8\A3\D9\83\D8\A8\D8\B1"(i64 %"\D8\A3", i64 %"\D8\A8") {
entry:
  %"\D8\A82" = alloca i64, align 8
  store i64 %"\D8\A8", ptr %"\D8\A82", align 8
  %"\D8\A31" = alloca i64, align 8
  store i64 %"\D8\A3", ptr %"\D8\A31", align 8
  %"%\D8\A3.load" = load i64, ptr %"\D8\A31", align 8
  %"%\D8\A8.load" = load i64, ptr %"\D8\A82", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\A3.load", %"%\D8\A8.load"
  br i1 %cmpgttmp, label %then_3, label %merge_4

then_3:                                           ; preds = %entry
  %loadtmp = load i64, ptr %"\D8\A31", align 8
  ret i64 %loadtmp

merge_4:                                          ; preds = %entry
  %loadtmp3 = load i64, ptr %"\D8\A82", align 8
  ret i64 %loadtmp3
}

define i64 @"\D8\B7\D8\A8\D9\82_\D8\A7\D8\B3\D8\AA\D8\B1\D8\A7\D8\AA\D9\8A\D8\AC\D9\8A\D8\A9"(ptr %"\D9\86\D9\88\D8\B9_\D8\A5", i64 %"\D8\A3", i64 %"\D8\A8") {
entry:
  %"\D8\A83" = alloca i64, align 8
  store i64 %"\D8\A8", ptr %"\D8\A83", align 8
  %"\D8\A32" = alloca i64, align 8
  store i64 %"\D8\A3", ptr %"\D8\A32", align 8
  %"\D9\86\D9\88\D8\B9_\D8\A51" = alloca ptr, align 8
  store ptr %"\D9\86\D9\88\D8\B9_\D8\A5", ptr %"\D9\86\D9\88\D8\B9_\D8\A51", align 8
  %loadtmp = load ptr, ptr %"\D9\86\D9\88\D8\B9_\D8\A51", align 8
  br label %match.case0.test_6

match.end_5:                                      ; preds = %match.case3.test_12
  ret i64 0

match.case0.test_6:                               ; preds = %entry
  %strcmp.ret = call i32 @strcmp(ptr %loadtmp, ptr @0)
  %streq = icmp eq i32 %strcmp.ret, 0
  br i1 %streq, label %match.case0.body_7, label %match.case1.test_8

match.case0.body_7:                               ; preds = %match.case0.test_6
  %"%\D8\A3.load" = load i64, ptr %"\D8\A32", align 8
  %"%\D8\A8.load" = load i64, ptr %"\D8\A83", align 8
  %calltmp = call i64 @"\D8\A7\D8\B3\D8\AA\D8\B1\D8\A7\D8\AA\D9\8A\D8\AC\D9\8A\D8\A9_\D8\AC\D9\85\D8\B9"(i64 %"%\D8\A3.load", i64 %"%\D8\A8.load")
  ret i64 %calltmp

match.case1.test_8:                               ; preds = %match.case0.test_6
  %strcmp.ret4 = call i32 @strcmp(ptr %loadtmp, ptr @1)
  %streq5 = icmp eq i32 %strcmp.ret4, 0
  br i1 %streq5, label %match.case1.body_9, label %match.case2.test_10

match.case1.body_9:                               ; preds = %match.case1.test_8
  %"%\D8\A3.load6" = load i64, ptr %"\D8\A32", align 8
  %"%\D8\A8.load7" = load i64, ptr %"\D8\A83", align 8
  %calltmp8 = call i64 @"\D8\A7\D8\B3\D8\AA\D8\B1\D8\A7\D8\AA\D9\8A\D8\AC\D9\8A\D8\A9_\D8\B6\D8\B1\D8\A8"(i64 %"%\D8\A3.load6", i64 %"%\D8\A8.load7")
  ret i64 %calltmp8

match.case2.test_10:                              ; preds = %match.case1.test_8
  %strcmp.ret9 = call i32 @strcmp(ptr %loadtmp, ptr @2)
  %streq10 = icmp eq i32 %strcmp.ret9, 0
  br i1 %streq10, label %match.case2.body_11, label %match.case3.test_12

match.case2.body_11:                              ; preds = %match.case2.test_10
  %"%\D8\A3.load11" = load i64, ptr %"\D8\A32", align 8
  %"%\D8\A8.load12" = load i64, ptr %"\D8\A83", align 8
  %calltmp13 = call i64 @"\D8\A7\D8\B3\D8\AA\D8\B1\D8\A7\D8\AA\D9\8A\D8\AC\D9\8A\D8\A9_\D8\A3\D9\83\D8\A8\D8\B1"(i64 %"%\D8\A3.load11", i64 %"%\D8\A8.load12")
  ret i64 %calltmp13

match.case3.test_12:                              ; preds = %match.case2.test_10
  br i1 true, label %match.case3.body_13, label %match.end_5

match.case3.body_13:                              ; preds = %match.case3.test_12
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.24)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  ret i64 0
}

define void @"\D8\A8\D9\86\D8\A7\D8\A1_\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, ptr %"\D8\A7\D9\84\D8\B9\D9\86\D9\88\D8\A7\D9\86") {
entry:
  %"\D8\A7\D9\84\D8\B9\D9\86\D9\88\D8\A7\D9\862" = alloca ptr, align 8
  store ptr %"\D8\A7\D9\84\D8\B9\D9\86\D9\88\D8\A7\D9\86", ptr %"\D8\A7\D9\84\D8\B9\D9\86\D9\88\D8\A7\D9\862", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca3 = alloca ptr, align 8
  %alloca4 = alloca ptr, align 8
  %alloca5 = alloca i64, align 8
  store ptr %"\D8\A7\D9\84\D8\B9\D9\86\D9\88\D8\A7\D9\862", ptr %alloca, align 8
  store ptr @str.const.28, ptr %alloca3, align 8
  store ptr @str.const.29, ptr %alloca4, align 8
  store i64 0, ptr %alloca5, align 8
  ret void
}

define void @"\D8\A8\D9\86\D8\A7\D8\A1_\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.\D8\AD\D8\AF\D8\AF_\D9\85\D8\B1\D8\B3\D9\84"(i64 %self, ptr %"\D8\A7\D9\84\D9\85\D8\B1\D8\B3\D9\84") {
entry:
  %"\D8\A7\D9\84\D9\85\D8\B1\D8\B3\D9\842" = alloca ptr, align 8
  store ptr %"\D8\A7\D9\84\D9\85\D8\B1\D8\B3\D9\84", ptr %"\D8\A7\D9\84\D9\85\D8\B1\D8\B3\D9\842", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca3 = alloca ptr, align 8
  %alloca4 = alloca ptr, align 8
  %alloca5 = alloca i64, align 8
  store ptr %"\D8\A7\D9\84\D9\85\D8\B1\D8\B3\D9\842", ptr %alloca3, align 8
  ret void
}

define void @"\D8\A8\D9\86\D8\A7\D8\A1_\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.\D8\AD\D8\AF\D8\AF_\D9\85\D8\AD\D8\AA\D9\88\D9\89"(i64 %self, ptr %"\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89") {
entry:
  %"\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\892" = alloca ptr, align 8
  store ptr %"\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89", ptr %"\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\892", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca3 = alloca ptr, align 8
  %alloca4 = alloca ptr, align 8
  %alloca5 = alloca i64, align 8
  store ptr %"\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\892", ptr %alloca4, align 8
  ret void
}

define void @"\D8\A8\D9\86\D8\A7\D8\A1_\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.\D8\AD\D8\AF\D8\AF_\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9"(i64 %self, i64 %"\D8\A7\D9\84\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9") {
entry:
  %"\D8\A7\D9\84\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A92" = alloca i64, align 8
  store i64 %"\D8\A7\D9\84\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9", ptr %"\D8\A7\D9\84\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A92", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca3 = alloca ptr, align 8
  %alloca4 = alloca ptr, align 8
  %alloca5 = alloca i64, align 8
  %"%\D8\A7\D9\84\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9.load" = load i64, ptr %"\D8\A7\D9\84\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A92", align 8
  store i64 %"%\D8\A7\D9\84\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9.load", ptr %alloca5, align 8
  ret void
}

define void @"\D8\A8\D9\86\D8\A7\D8\A1_\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.\D8\A7\D8\A8\D9\86\D9\90"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca ptr, align 8
  %alloca4 = alloca i64, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.30)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %len1 = call i64 @strlen(ptr @str.const.34)
  %len2 = call i64 @strlen(ptr %alloca)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %2 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.34, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %3 = call ptr @memcpy(ptr %dest2, ptr %alloca, i64 %copylen2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %len15 = call i64 @strlen(ptr @str.const.38)
  %len26 = call i64 @strlen(ptr %alloca2)
  %totallen7 = add i64 %len15, %len26
  %bufsize8 = add i64 %totallen7, 1
  %concat.buf9 = alloca i8, i64 %bufsize8, align 1
  %6 = call ptr @memcpy(ptr %concat.buf9, ptr @str.const.38, i64 %len15)
  %dest210 = getelementptr i8, ptr %concat.buf9, i64 %len15
  %copylen211 = add i64 %len26, 1
  %7 = call ptr @memcpy(ptr %dest210, ptr %alloca2, i64 %copylen211)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf9)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %len112 = call i64 @strlen(ptr @str.const.45)
  %len213 = call i64 @strlen(ptr %alloca3)
  %totallen14 = add i64 %len112, %len213
  %bufsize15 = add i64 %totallen14, 1
  %concat.buf16 = alloca i8, i64 %bufsize15, align 1
  %11 = call ptr @memcpy(ptr %concat.buf16, ptr @str.const.45, i64 %len112)
  %dest217 = getelementptr i8, ptr %concat.buf16, i64 %len112
  %copylen218 = add i64 %len213, 1
  %12 = call ptr @memcpy(ptr %dest217, ptr %alloca3, i64 %copylen218)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf16)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.30)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  ret void
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.53)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.57)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.61)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %alloca = alloca i64, align 8
  %"%76.load" = load i64, ptr %alloca, align 8
  call void @"\D9\85\D8\B1\D8\A7\D9\82\D8\A8.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%76.load", ptr @str.const.65)
  %alloca1 = alloca i64, align 8
  %"%76.load2" = load i64, ptr %alloca, align 8
  store i64 %"%76.load2", ptr %alloca1, align 8
  %alloca3 = alloca i64, align 8
  %"%79.load" = load i64, ptr %alloca3, align 8
  call void @"\D9\85\D8\B1\D8\A7\D9\82\D8\A8.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%79.load", ptr @str.const.66)
  %alloca4 = alloca i64, align 8
  %"%79.load5" = load i64, ptr %alloca3, align 8
  store i64 %"%79.load5", ptr %alloca4, align 8
  %"%\D9\851.load" = load i64, ptr %alloca1, align 8
  call void @"\D9\85\D8\B1\D8\A7\D9\82\D8\A8.\D8\B9\D8\A7\D9\84\D8\AC"(i64 %"%\D9\851.load", ptr @str.const.67, ptr @str.const.68)
  %"%\D9\852.load" = load i64, ptr %alloca4, align 8
  call void @"\D9\85\D8\B1\D8\A7\D9\82\D8\A8.\D8\B9\D8\A7\D9\84\D8\AC"(i64 %"%\D9\852.load", ptr @str.const.67, ptr @str.const.68)
  %"%\D9\851.load6" = load i64, ptr %alloca1, align 8
  call void @"\D9\85\D8\B1\D8\A7\D9\82\D8\A8.\D8\B9\D8\A7\D9\84\D8\AC"(i64 %"%\D9\851.load6", ptr @str.const.71, ptr @str.const.72)
  %"%\D9\852.load7" = load i64, ptr %alloca4, align 8
  call void @"\D9\85\D8\B1\D8\A7\D9\82\D8\A8.\D8\B9\D8\A7\D9\84\D8\AC"(i64 %"%\D9\852.load7", ptr @str.const.71, ptr @str.const.72)
  %"%\D9\851.load8" = load i64, ptr %alloca1, align 8
  %calltmp = call i64 @"\D9\85\D8\B1\D8\A7\D9\82\D8\A8.\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A7\D8\AA"(i64 %"%\D9\851.load8")
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %"%\D9\852.load9" = load i64, ptr %alloca4, align 8
  %calltmp10 = call i64 @"\D9\85\D8\B1\D8\A7\D9\82\D8\A8.\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A7\D8\AA"(i64 %"%\D9\852.load9")
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.83)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.87)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %alloca11 = alloca i64, align 8
  %"%110.load" = load i64, ptr %alloca11, align 8
  call void @"\D8\A3\D9\85\D8\B1.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%110.load", ptr @str.const.91, ptr @str.const.92)
  %alloca12 = alloca i64, align 8
  %"%110.load13" = load i64, ptr %alloca11, align 8
  store i64 %"%110.load13", ptr %alloca12, align 8
  %alloca14 = alloca i64, align 8
  %"%114.load" = load i64, ptr %alloca14, align 8
  call void @"\D8\A3\D9\85\D8\B1.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%114.load", ptr @str.const.93, ptr @str.const.94)
  %alloca15 = alloca i64, align 8
  %"%114.load16" = load i64, ptr %alloca14, align 8
  store i64 %"%114.load16", ptr %alloca15, align 8
  %alloca17 = alloca i64, align 8
  %"%118.load" = load i64, ptr %alloca17, align 8
  call void @"\D8\A3\D9\85\D8\B1.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%118.load", ptr @str.const.95, ptr @str.const.96)
  %alloca18 = alloca i64, align 8
  %"%118.load19" = load i64, ptr %alloca17, align 8
  store i64 %"%118.load19", ptr %alloca18, align 8
  %"%\D8\A31.load" = load i64, ptr %alloca12, align 8
  call void @"\D8\A3\D9\85\D8\B1.\D8\B4\D8\BA\D9\91\D9\84"(i64 %"%\D8\A31.load")
  %"%\D8\A32.load" = load i64, ptr %alloca15, align 8
  call void @"\D8\A3\D9\85\D8\B1.\D8\B4\D8\BA\D9\91\D9\84"(i64 %"%\D8\A32.load")
  %"%\D8\A33.load" = load i64, ptr %alloca18, align 8
  call void @"\D8\A3\D9\85\D8\B1.\D8\B4\D8\BA\D9\91\D9\84"(i64 %"%\D8\A33.load")
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.97)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %"%\D8\A33.load20" = load i64, ptr %alloca18, align 8
  call void @"\D8\A3\D9\85\D8\B1.\D8\AA\D8\B1\D8\A7\D8\AC\D8\B9"(i64 %"%\D8\A33.load20")
  %"%\D8\A32.load21" = load i64, ptr %alloca15, align 8
  call void @"\D8\A3\D9\85\D8\B1.\D8\AA\D8\B1\D8\A7\D8\AC\D8\B9"(i64 %"%\D8\A32.load21")
  %"%\D8\A31.load22" = load i64, ptr %alloca12, align 8
  call void @"\D8\A3\D9\85\D8\B1.\D8\AA\D8\B1\D8\A7\D8\AC\D8\B9"(i64 %"%\D8\A31.load22")
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.101)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.105)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %alloca23 = alloca i64, align 8
  %"%131.load" = load i64, ptr %alloca23, align 8
  call void @"\D8\A2\D9\84\D8\A9_\D8\AD\D8\A7\D9\84\D8\A7\D8\AA.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%131.load", ptr @str.const.109)
  %alloca24 = alloca i64, align 8
  %"%131.load25" = load i64, ptr %alloca23, align 8
  store i64 %"%131.load25", ptr %alloca24, align 8
  %"%\D8\A2\D9\84\D8\A9.load" = load i64, ptr %alloca24, align 8
  call void @"\D8\A2\D9\84\D8\A9_\D8\AD\D8\A7\D9\84\D8\A7\D8\AA.\D8\A7\D9\86\D8\AA\D9\82\D9\84"(i64 %"%\D8\A2\D9\84\D8\A9.load", ptr @str.const.110)
  %"%\D8\A2\D9\84\D8\A9.load26" = load i64, ptr %alloca24, align 8
  call void @"\D8\A2\D9\84\D8\A9_\D8\AD\D8\A7\D9\84\D8\A7\D8\AA.\D8\A7\D9\86\D8\AA\D9\82\D9\84"(i64 %"%\D8\A2\D9\84\D8\A9.load26", ptr @str.const.111)
  %"%\D8\A2\D9\84\D8\A9.load27" = load i64, ptr %alloca24, align 8
  call void @"\D8\A2\D9\84\D8\A9_\D8\AD\D8\A7\D9\84\D8\A7\D8\AA.\D8\A7\D9\86\D8\AA\D9\82\D9\84"(i64 %"%\D8\A2\D9\84\D8\A9.load27", ptr @str.const.112)
  %"%\D8\A2\D9\84\D8\A9.load28" = load i64, ptr %alloca24, align 8
  call void @"\D8\A2\D9\84\D8\A9_\D8\AD\D8\A7\D9\84\D8\A7\D8\AA.\D8\A7\D9\86\D8\AA\D9\82\D9\84"(i64 %"%\D8\A2\D9\84\D8\A9.load28", ptr @str.const.113)
  %"%\D8\A2\D9\84\D8\A9.load29" = load i64, ptr %alloca24, align 8
  %calltmp30 = call ptr @"\D8\A2\D9\84\D8\A9_\D8\AD\D8\A7\D9\84\D8\A7\D8\AA.\D8\A7\D9\84\D8\AD\D8\A7\D9\84"(i64 %"%\D8\A2\D9\84\D8\A9.load29")
  %len1 = call i64 @strlen(ptr @str.const.114)
  %len2 = call i64 @strlen(ptr %calltmp30)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %18 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.114, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %19 = call ptr @memcpy(ptr %dest2, ptr %calltmp30, i64 %copylen2)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %"%\D8\A2\D9\84\D8\A9.load31" = load i64, ptr %alloca24, align 8
  %calltmp32 = call i64 @"\D8\A2\D9\84\D8\A9_\D8\AD\D8\A7\D9\84\D8\A7\D8\AA.\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\AA\D8\BA\D9\8A\D9\8A\D8\B1\D8\A7\D8\AA"(i64 %"%\D8\A2\D9\84\D8\A9.load31")
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.121)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.125)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %calltmp33 = call i64 @"\D8\B7\D8\A8\D9\82_\D8\A7\D8\B3\D8\AA\D8\B1\D8\A7\D8\AA\D9\8A\D8\AC\D9\8A\D8\A9"(ptr @0, i64 10, i64 20)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %calltmp34 = call i64 @"\D8\B7\D8\A8\D9\82_\D8\A7\D8\B3\D8\AA\D8\B1\D8\A7\D8\AA\D9\8A\D8\AC\D9\8A\D8\A9"(ptr @1, i64 10, i64 20)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %calltmp35 = call i64 @"\D8\B7\D8\A8\D9\82_\D8\A7\D8\B3\D8\AA\D8\B1\D8\A7\D8\AA\D9\8A\D8\AC\D9\8A\D8\A9"(ptr @2, i64 10, i64 20)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.141)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.145)
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %alloca36 = alloca i64, align 8
  %"%174.load" = load i64, ptr %alloca36, align 8
  call void @"\D8\A8\D9\86\D8\A7\D8\A1_\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%174.load", ptr @str.const.149)
  %alloca37 = alloca i64, align 8
  %"%174.load38" = load i64, ptr %alloca36, align 8
  store i64 %"%174.load38", ptr %alloca37, align 8
  %"%\D8\B11.load" = load i64, ptr %alloca37, align 8
  call void @"\D8\A8\D9\86\D8\A7\D8\A1_\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.\D8\AD\D8\AF\D8\AF_\D9\85\D8\B1\D8\B3\D9\84"(i64 %"%\D8\B11.load", ptr @str.const.150)
  %"%\D8\B11.load39" = load i64, ptr %alloca37, align 8
  call void @"\D8\A8\D9\86\D8\A7\D8\A1_\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.\D8\AD\D8\AF\D8\AF_\D9\85\D8\AD\D8\AA\D9\88\D9\89"(i64 %"%\D8\B11.load39", ptr @str.const.151)
  %"%\D8\B11.load40" = load i64, ptr %alloca37, align 8
  call void @"\D8\A8\D9\86\D8\A7\D8\A1_\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.\D8\AD\D8\AF\D8\AF_\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9"(i64 %"%\D8\B11.load40", i64 2)
  %"%\D8\B11.load41" = load i64, ptr %alloca37, align 8
  call void @"\D8\A8\D9\86\D8\A7\D8\A1_\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.\D8\A7\D8\A8\D9\86\D9\90"(i64 %"%\D8\B11.load41")
  %34 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.152)
  %35 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %alloca42 = alloca i64, align 8
  %"%185.load" = load i64, ptr %alloca42, align 8
  call void @"\D8\A8\D9\86\D8\A7\D8\A1_\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%185.load", ptr @str.const.156)
  %alloca43 = alloca i64, align 8
  %"%185.load44" = load i64, ptr %alloca42, align 8
  store i64 %"%185.load44", ptr %alloca43, align 8
  %"%\D8\B12.load" = load i64, ptr %alloca43, align 8
  call void @"\D8\A8\D9\86\D8\A7\D8\A1_\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.\D8\AD\D8\AF\D8\AF_\D9\85\D8\B1\D8\B3\D9\84"(i64 %"%\D8\B12.load", ptr @str.const.157)
  %"%\D8\B12.load45" = load i64, ptr %alloca43, align 8
  call void @"\D8\A8\D9\86\D8\A7\D8\A1_\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.\D8\AD\D8\AF\D8\AF_\D9\85\D8\AD\D8\AA\D9\88\D9\89"(i64 %"%\D8\B12.load45", ptr @str.const.158)
  %"%\D8\B12.load46" = load i64, ptr %alloca43, align 8
  call void @"\D8\A8\D9\86\D8\A7\D8\A1_\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.\D8\AD\D8\AF\D8\AF_\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9"(i64 %"%\D8\B12.load46", i64 5)
  %"%\D8\B12.load47" = load i64, ptr %alloca43, align 8
  call void @"\D8\A8\D9\86\D8\A7\D8\A1_\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.\D8\A7\D8\A8\D9\86\D9\90"(i64 %"%\D8\B12.load47")
  %36 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.159)
  %37 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %38 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.163)
  %39 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  ret i64 0
}

declare i64 @strlen(ptr)

declare ptr @memcpy(ptr, ptr, i64)

declare i32 @printf(ptr, ...)

declare i32 @strcmp(ptr, ptr)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
