; ModuleID = '01_simple_vm_interpreter'
source_filename = "01_simple_vm_interpreter"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [4 x i8] c"  [\00", align 1
@str.const.1 = private unnamed_addr constant [3 x i8] c"] \00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.2 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.4 = private unnamed_addr constant [9 x i8] c"    \E2\86\90 \00", align 1
@str.const.5 = private unnamed_addr constant [4 x i8] c" = \00", align 1
@str.ret = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.8 = private unnamed_addr constant [34 x i8] c"=== \D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9 \D8\A7\D9\84\D9\85\D9\83\D8\AF\D8\B3 ===\00", align 1
@str.const.12 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.16 = private unnamed_addr constant [5 x i8] c"PUSH\00", align 1
@str.const.17 = private unnamed_addr constant [10 x i8] c"\D8\AF\D9\81\D8\B9 10\00", align 1
@str.const.18 = private unnamed_addr constant [22 x i8] c"\D8\AF\D9\84\D9\8A\D9\84 \D8\A7\D9\84\D9\85\D9\83\D8\AF\D8\B3\00", align 1
@str.const.20 = private unnamed_addr constant [10 x i8] c"\D8\AF\D9\81\D8\B9 20\00", align 1
@str.const.23 = private unnamed_addr constant [10 x i8] c"\D8\AF\D9\81\D8\B9 30\00", align 1
@str.const.25 = private unnamed_addr constant [4 x i8] c"ADD\00", align 1
@str.const.26 = private unnamed_addr constant [22 x i8] c"\D8\AC\D9\85\D8\B9 \D8\A7\D9\84\D9\82\D9\85\D8\AA\D9\8A\D9\86\00", align 1
@str.const.27 = private unnamed_addr constant [22 x i8] c"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9 \D8\A7\D9\84\D8\AC\D9\85\D8\B9\00", align 1
@str.const.28 = private unnamed_addr constant [4 x i8] c"MUL\00", align 1
@str.const.29 = private unnamed_addr constant [22 x i8] c"\D8\B6\D8\B1\D8\A8 \D8\A7\D9\84\D9\82\D9\85\D8\AA\D9\8A\D9\86\00", align 1
@str.const.30 = private unnamed_addr constant [22 x i8] c"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9 \D8\A7\D9\84\D8\B6\D8\B1\D8\A8\00", align 1
@str.const.31 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.35 = private unnamed_addr constant [36 x i8] c"  \D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9 \D8\A7\D9\84\D9\86\D9\87\D8\A7\D8\A6\D9\8A\D8\A9: \00", align 1
@str.const.39 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.43 = private unnamed_addr constant [36 x i8] c"=== \D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9 \D8\A7\D9\84\D8\B3\D8\AC\D9\84\D8\A7\D8\AA ===\00", align 1
@str.const.47 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.51 = private unnamed_addr constant [5 x i8] c"LOAD\00", align 1
@str.const.52 = private unnamed_addr constant [9 x i8] c"R0 = 100\00", align 1
@str.const.53 = private unnamed_addr constant [3 x i8] c"R0\00", align 1
@str.const.55 = private unnamed_addr constant [8 x i8] c"R1 = 50\00", align 1
@str.const.56 = private unnamed_addr constant [3 x i8] c"R1\00", align 1
@str.const.58 = private unnamed_addr constant [13 x i8] c"R2 = R0 + R1\00", align 1
@str.const.59 = private unnamed_addr constant [3 x i8] c"R2\00", align 1
@str.const.60 = private unnamed_addr constant [4 x i8] c"SUB\00", align 1
@str.const.61 = private unnamed_addr constant [13 x i8] c"R3 = R0 - R1\00", align 1
@str.const.62 = private unnamed_addr constant [3 x i8] c"R3\00", align 1
@str.const.64 = private unnamed_addr constant [13 x i8] c"R0 = R2 * R3\00", align 1
@str.const.66 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.70 = private unnamed_addr constant [35 x i8] c"  \D8\A7\D9\84\D9\86\D8\AA\D8\A7\D8\A6\D8\AC \D8\A7\D9\84\D9\86\D9\87\D8\A7\D8\A6\D9\8A\D8\A9:\00", align 1
@str.const.74 = private unnamed_addr constant [10 x i8] c"    R0 = \00", align 1
@str.const.78 = private unnamed_addr constant [10 x i8] c"    R1 = \00", align 1
@str.const.82 = private unnamed_addr constant [10 x i8] c"    R2 = \00", align 1
@str.const.86 = private unnamed_addr constant [10 x i8] c"    R3 = \00", align 1
@str.const.90 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.94 = private unnamed_addr constant [43 x i8] c"=== \D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9 \D8\AD\D9\84\D9\82\D8\A9 \D8\A8\D8\B1\D9\86\D8\A7\D9\85\D8\AC ===\00", align 1
@str.const.98 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.102 = private unnamed_addr constant [15 x i8] c"\D9\85\D8\AC\D9\85\D9\88\D8\B9 += \00", align 1
@str.const.104 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.108 = private unnamed_addr constant [20 x i8] c"  \D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9 = \00", align 1
@str.const.112 = private unnamed_addr constant [39 x i8] c"  \D8\A7\D9\84\D8\AA\D8\B9\D9\84\D9\8A\D9\85\D8\A7\D8\AA \D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0\D8\A9 = \00", align 1
@str.const.116 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.120 = private unnamed_addr constant [57 x i8] c"=== \D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9 \D8\A7\D9\84\D8\AA\D8\B9\D9\84\D9\8A\D9\85\D8\A7\D8\AA \D8\A7\D9\84\D9\85\D8\B4\D8\B1\D9\88\D8\B7\D8\A9 ===\00", align 1
@str.const.124 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.128 = private unnamed_addr constant [14 x i8] c"\D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A9 \00", align 1
@str.const.129 = private unnamed_addr constant [7 x i8] c" \D9\85\D8\B9 \00", align 1
@str.const.130 = private unnamed_addr constant [4 x i8] c"CMP\00", align 1
@str.const.131 = private unnamed_addr constant [4 x i8] c"JEQ\00", align 1
@str.const.132 = private unnamed_addr constant [30 x i8] c"\D8\A7\D9\84\D9\82\D9\81\D8\B2 - \D9\85\D8\AA\D8\B3\D8\A7\D9\88\D9\8A\D8\A7\D9\86\00", align 1
@str.const.133 = private unnamed_addr constant [4 x i8] c"JGT\00", align 1
@str.const.134 = private unnamed_addr constant [30 x i8] c"\D8\A7\D9\84\D9\82\D9\81\D8\B2 - \D8\A3\D9\83\D8\A8\D8\B1 \D9\85\D9\86 20\00", align 1
@str.const.135 = private unnamed_addr constant [4 x i8] c"JLT\00", align 1
@str.const.136 = private unnamed_addr constant [31 x i8] c"\D8\A7\D9\84\D9\82\D9\81\D8\B2 - \D8\A3\D8\B5\D8\BA\D8\B1 \D9\85\D9\86 100\00", align 1
@str.const.137 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.141 = private unnamed_addr constant [26 x i8] c"  \D8\B9\D8\AF\D8\AF \D8\A7\D9\84\D9\82\D9\81\D8\B2\D8\A7\D8\AA: \00", align 1
@str.const.145 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.149 = private unnamed_addr constant [121 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.153 = private unnamed_addr constant [66 x i8] c"\E2\95\91     \D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9 \D8\A2\D9\84\D8\A9 \D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A\D8\A9 \D8\A8\D8\B3\D9\8A\D8\B7\D8\A9       \E2\95\91\00", align 1
@str.const.157 = private unnamed_addr constant [121 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.161 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.165 = private unnamed_addr constant [36 x i8] c"=== \D8\A7\D9\86\D8\AA\D9\87\D9\89 \D8\A7\D9\84\D8\A8\D8\B1\D9\86\D8\A7\D9\85\D8\AC ===\00", align 1

define i64 @"\D8\B3\D8\AC_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(ptr %"\D8\B9\D9\85\D9\84\D9\8A\D8\A9", ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\84") {
entry:
  %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\842" = alloca ptr, align 8
  store ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\84", ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\842", align 8
  %"\D8\B9\D9\85\D9\84\D9\8A\D8\A91" = alloca ptr, align 8
  store ptr %"\D8\B9\D9\85\D9\84\D9\8A\D8\A9", ptr %"\D8\B9\D9\85\D9\84\D9\8A\D8\A91", align 8
  %len1 = call i64 @strlen(ptr @str.const)
  %len2 = call i64 @strlen(ptr %"\D8\B9\D9\85\D9\84\D9\8A\D8\A91")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\B9\D9\85\D9\84\D9\8A\D8\A91", i64 %copylen2)
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

define ptr @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr %"\D9\88\D8\B5\D9\81", i64 %"\D9\82\D9\8A\D9\85\D8\AA\D9\87") {
entry:
  %"\D9\82\D9\8A\D9\85\D8\AA\D9\872" = alloca i64, align 8
  store i64 %"\D9\82\D9\8A\D9\85\D8\AA\D9\87", ptr %"\D9\82\D9\8A\D9\85\D8\AA\D9\872", align 8
  %"\D9\88\D8\B5\D9\811" = alloca ptr, align 8
  store ptr %"\D9\88\D8\B5\D9\81", ptr %"\D9\88\D8\B5\D9\811", align 8
  %len1 = call i64 @strlen(ptr @str.const.4)
  %len2 = call i64 @strlen(ptr %"\D9\88\D8\B5\D9\811")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.4, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D9\88\D8\B5\D9\811", i64 %copylen2)
  %len13 = call i64 @strlen(ptr %concat.buf)
  %len24 = call i64 @strlen(ptr @str.const.5)
  %totallen5 = add i64 %len13, %len24
  %bufsize6 = add i64 %totallen5, 1
  %concat.buf7 = alloca i8, i64 %bufsize6, align 1
  %2 = call ptr @memcpy(ptr %concat.buf7, ptr %concat.buf, i64 %len13)
  %dest28 = getelementptr i8, ptr %concat.buf7, i64 %len13
  %copylen29 = add i64 %len24, 1
  %3 = call ptr @memcpy(ptr %dest28, ptr @str.const.5, i64 %copylen29)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret ptr @str.ret
}

define i64 @"\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9_\D9\85\D9\83\D8\AF\D8\B3"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.8)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 0, ptr %alloca1, align 8
  %alloca2 = alloca i64, align 8
  store i64 0, ptr %alloca2, align 8
  %alloca3 = alloca i64, align 8
  store i64 0, ptr %alloca3, align 8
  %calltmp = call i64 @"\D8\B3\D8\AC_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(ptr @str.const.16, ptr @str.const.17)
  store i64 10, ptr %alloca, align 8
  store i64 1, ptr %alloca3, align 8
  %"%\D8\AF\D9\84\D9\8A\D9\84_\D9\85.load" = load i64, ptr %alloca3, align 8
  %calltmp4 = call ptr @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.18, i64 %"%\D8\AF\D9\84\D9\8A\D9\84_\D9\85.load")
  %calltmp5 = call i64 @"\D8\B3\D8\AC_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(ptr @str.const.16, ptr @str.const.20)
  store i64 20, ptr %alloca1, align 8
  store i64 2, ptr %alloca3, align 8
  %"%\D8\AF\D9\84\D9\8A\D9\84_\D9\85.load6" = load i64, ptr %alloca3, align 8
  %calltmp7 = call ptr @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.18, i64 %"%\D8\AF\D9\84\D9\8A\D9\84_\D9\85.load6")
  %calltmp8 = call i64 @"\D8\B3\D8\AC_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(ptr @str.const.16, ptr @str.const.23)
  store i64 30, ptr %alloca2, align 8
  store i64 3, ptr %alloca3, align 8
  %"%\D8\AF\D9\84\D9\8A\D9\84_\D9\85.load9" = load i64, ptr %alloca3, align 8
  %calltmp10 = call ptr @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.18, i64 %"%\D8\AF\D9\84\D9\8A\D9\84_\D9\85.load9")
  %calltmp11 = call i64 @"\D8\B3\D8\AC_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(ptr @str.const.25, ptr @str.const.26)
  %"%\D9\851.load" = load i64, ptr %alloca1, align 8
  %"%\D9\852.load" = load i64, ptr %alloca2, align 8
  %addtmp = add i64 %"%\D9\851.load", %"%\D9\852.load"
  %alloca12 = alloca i64, align 8
  store i64 %addtmp, ptr %alloca12, align 8
  store i64 2, ptr %alloca3, align 8
  %"%\D8\AD\D8\A7\D8\B5\D9\84.load" = load i64, ptr %alloca12, align 8
  %calltmp13 = call ptr @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.27, i64 %"%\D8\AD\D8\A7\D8\B5\D9\84.load")
  %calltmp14 = call i64 @"\D8\B3\D8\AC_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(ptr @str.const.28, ptr @str.const.29)
  %"%\D9\850.load" = load i64, ptr %alloca, align 8
  %"%\D8\AD\D8\A7\D8\B5\D9\84.load15" = load i64, ptr %alloca12, align 8
  %multmp = mul i64 %"%\D9\850.load", %"%\D8\AD\D8\A7\D8\B5\D9\84.load15"
  %alloca16 = alloca i64, align 8
  store i64 %multmp, ptr %alloca16, align 8
  store i64 1, ptr %alloca3, align 8
  %"%\D9\86\D8\A7\D8\AA\D8\AC.load" = load i64, ptr %alloca16, align 8
  %calltmp17 = call ptr @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.30, i64 %"%\D9\86\D8\A7\D8\AA\D8\AC.load")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.31)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca18 = alloca ptr, align 8
  %len1 = call i64 @strlen(ptr @str.const.35)
  %len2 = call i64 @strlen(ptr %alloca18)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %6 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.35, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %7 = call ptr @memcpy(ptr %dest2, ptr %alloca18, i64 %copylen2)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.39)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9_\D8\B3\D8\AC\D9\84\D8\A7\D8\AA"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.43)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.47)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 0, ptr %alloca1, align 8
  %alloca2 = alloca i64, align 8
  store i64 0, ptr %alloca2, align 8
  %alloca3 = alloca i64, align 8
  store i64 0, ptr %alloca3, align 8
  %calltmp = call i64 @"\D8\B3\D8\AC_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(ptr @str.const.51, ptr @str.const.52)
  store i64 100, ptr %alloca, align 8
  %"%\D8\B10.load" = load i64, ptr %alloca, align 8
  %calltmp4 = call ptr @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.53, i64 %"%\D8\B10.load")
  %calltmp5 = call i64 @"\D8\B3\D8\AC_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(ptr @str.const.51, ptr @str.const.55)
  store i64 50, ptr %alloca1, align 8
  %"%\D8\B11.load" = load i64, ptr %alloca1, align 8
  %calltmp6 = call ptr @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.56, i64 %"%\D8\B11.load")
  %calltmp7 = call i64 @"\D8\B3\D8\AC_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(ptr @str.const.25, ptr @str.const.58)
  %"%\D8\B10.load8" = load i64, ptr %alloca, align 8
  %"%\D8\B11.load9" = load i64, ptr %alloca1, align 8
  %addtmp = add i64 %"%\D8\B10.load8", %"%\D8\B11.load9"
  store i64 %addtmp, ptr %alloca2, align 8
  %"%\D8\B12.load" = load i64, ptr %alloca2, align 8
  %calltmp10 = call ptr @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.59, i64 %"%\D8\B12.load")
  %calltmp11 = call i64 @"\D8\B3\D8\AC_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(ptr @str.const.60, ptr @str.const.61)
  %"%\D8\B10.load12" = load i64, ptr %alloca, align 8
  %"%\D8\B11.load13" = load i64, ptr %alloca1, align 8
  %subtmp = sub i64 %"%\D8\B10.load12", %"%\D8\B11.load13"
  store i64 %subtmp, ptr %alloca3, align 8
  %"%\D8\B13.load" = load i64, ptr %alloca3, align 8
  %calltmp14 = call ptr @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.62, i64 %"%\D8\B13.load")
  %calltmp15 = call i64 @"\D8\B3\D8\AC_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(ptr @str.const.28, ptr @str.const.64)
  %"%\D8\B12.load16" = load i64, ptr %alloca2, align 8
  %"%\D8\B13.load17" = load i64, ptr %alloca3, align 8
  %multmp = mul i64 %"%\D8\B12.load16", %"%\D8\B13.load17"
  store i64 %multmp, ptr %alloca, align 8
  %"%\D8\B10.load18" = load i64, ptr %alloca, align 8
  %calltmp19 = call ptr @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.53, i64 %"%\D8\B10.load18")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.66)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca20 = alloca ptr, align 8
  %alloca21 = alloca ptr, align 8
  %alloca22 = alloca ptr, align 8
  %alloca23 = alloca ptr, align 8
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.70)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %len1 = call i64 @strlen(ptr @str.const.74)
  %len2 = call i64 @strlen(ptr %alloca20)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %8 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.74, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %9 = call ptr @memcpy(ptr %dest2, ptr %alloca20, i64 %copylen2)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %len124 = call i64 @strlen(ptr @str.const.78)
  %len225 = call i64 @strlen(ptr %alloca21)
  %totallen26 = add i64 %len124, %len225
  %bufsize27 = add i64 %totallen26, 1
  %concat.buf28 = alloca i8, i64 %bufsize27, align 1
  %12 = call ptr @memcpy(ptr %concat.buf28, ptr @str.const.78, i64 %len124)
  %dest229 = getelementptr i8, ptr %concat.buf28, i64 %len124
  %copylen230 = add i64 %len225, 1
  %13 = call ptr @memcpy(ptr %dest229, ptr %alloca21, i64 %copylen230)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf28)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %len131 = call i64 @strlen(ptr @str.const.82)
  %len232 = call i64 @strlen(ptr %alloca22)
  %totallen33 = add i64 %len131, %len232
  %bufsize34 = add i64 %totallen33, 1
  %concat.buf35 = alloca i8, i64 %bufsize34, align 1
  %16 = call ptr @memcpy(ptr %concat.buf35, ptr @str.const.82, i64 %len131)
  %dest236 = getelementptr i8, ptr %concat.buf35, i64 %len131
  %copylen237 = add i64 %len232, 1
  %17 = call ptr @memcpy(ptr %dest236, ptr %alloca22, i64 %copylen237)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf35)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %len138 = call i64 @strlen(ptr @str.const.86)
  %len239 = call i64 @strlen(ptr %alloca23)
  %totallen40 = add i64 %len138, %len239
  %bufsize41 = add i64 %totallen40, 1
  %concat.buf42 = alloca i8, i64 %bufsize41, align 1
  %20 = call ptr @memcpy(ptr %concat.buf42, ptr @str.const.86, i64 %len138)
  %dest243 = getelementptr i8, ptr %concat.buf42, i64 %len138
  %copylen244 = add i64 %len239, 1
  %21 = call ptr @memcpy(ptr %dest243, ptr %alloca23, i64 %copylen244)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf42)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.90)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9_\D8\AD\D9\84\D9\82\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.94)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.98)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 1, ptr %alloca1, align 8
  %alloca2 = alloca i64, align 8
  store i64 0, ptr %alloca2, align 8
  br label %while_cond_0

while_cond_0:                                     ; preds = %while_body_1, %entry
  %"%\D8\B9\D8\AF\D8\A7\D8\AF.load" = load i64, ptr %alloca1, align 8
  %cmpletmp = icmp sle i64 %"%\D8\B9\D8\AF\D8\A7\D8\AF.load", 10
  br i1 %cmpletmp, label %while_body_1, label %while_exit_2

while_body_1:                                     ; preds = %while_cond_0
  %alloca3 = alloca ptr, align 8
  %len1 = call i64 @strlen(ptr @str.const.102)
  %len2 = call i64 @strlen(ptr %alloca3)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %4 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.102, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %5 = call ptr @memcpy(ptr %dest2, ptr %alloca3, i64 %copylen2)
  %calltmp = call i64 @"\D8\B3\D8\AC_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(ptr @str.const.25, ptr %concat.buf)
  %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9.load" = load i64, ptr %alloca, align 8
  %"%\D8\B9\D8\AF\D8\A7\D8\AF.load4" = load i64, ptr %alloca1, align 8
  %addtmp = add i64 %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9.load", %"%\D8\B9\D8\AF\D8\A7\D8\AF.load4"
  store i64 %addtmp, ptr %alloca, align 8
  %"%\D8\B9\D8\AF\D8\A7\D8\AF.load5" = load i64, ptr %alloca1, align 8
  %addtmp6 = add i64 %"%\D8\B9\D8\AF\D8\A7\D8\AF.load5", 1
  store i64 %addtmp6, ptr %alloca1, align 8
  %"%\D8\AA\D8\B9\D9\84\D9\8A\D9\85\D8\A7\D8\AA.load" = load i64, ptr %alloca2, align 8
  %addtmp7 = add i64 %"%\D8\AA\D8\B9\D9\84\D9\8A\D9\85\D8\A7\D8\AA.load", 2
  store i64 %addtmp7, ptr %alloca2, align 8
  br label %while_cond_0

while_exit_2:                                     ; preds = %while_cond_0
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.104)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca8 = alloca ptr, align 8
  %alloca9 = alloca ptr, align 8
  %len110 = call i64 @strlen(ptr @str.const.108)
  %len211 = call i64 @strlen(ptr %alloca8)
  %totallen12 = add i64 %len110, %len211
  %bufsize13 = add i64 %totallen12, 1
  %concat.buf14 = alloca i8, i64 %bufsize13, align 1
  %8 = call ptr @memcpy(ptr %concat.buf14, ptr @str.const.108, i64 %len110)
  %dest215 = getelementptr i8, ptr %concat.buf14, i64 %len110
  %copylen216 = add i64 %len211, 1
  %9 = call ptr @memcpy(ptr %dest215, ptr %alloca8, i64 %copylen216)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf14)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %len117 = call i64 @strlen(ptr @str.const.112)
  %len218 = call i64 @strlen(ptr %alloca9)
  %totallen19 = add i64 %len117, %len218
  %bufsize20 = add i64 %totallen19, 1
  %concat.buf21 = alloca i8, i64 %bufsize20, align 1
  %12 = call ptr @memcpy(ptr %concat.buf21, ptr @str.const.112, i64 %len117)
  %dest222 = getelementptr i8, ptr %concat.buf21, i64 %len117
  %copylen223 = add i64 %len218, 1
  %13 = call ptr @memcpy(ptr %dest222, ptr %alloca9, i64 %copylen223)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf21)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.116)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9_\D8\B4\D8\B1\D9\88\D8\B7"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.120)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.124)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca i64, align 8
  store i64 42, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 42, ptr %alloca1, align 8
  %alloca2 = alloca i64, align 8
  store i64 0, ptr %alloca2, align 8
  %alloca3 = alloca ptr, align 8
  %alloca4 = alloca ptr, align 8
  %len1 = call i64 @strlen(ptr @str.const.128)
  %len2 = call i64 @strlen(ptr %alloca3)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %4 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.128, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %5 = call ptr @memcpy(ptr %dest2, ptr %alloca3, i64 %copylen2)
  %len15 = call i64 @strlen(ptr %concat.buf)
  %len26 = call i64 @strlen(ptr @str.const.129)
  %totallen7 = add i64 %len15, %len26
  %bufsize8 = add i64 %totallen7, 1
  %concat.buf9 = alloca i8, i64 %bufsize8, align 1
  %6 = call ptr @memcpy(ptr %concat.buf9, ptr %concat.buf, i64 %len15)
  %dest210 = getelementptr i8, ptr %concat.buf9, i64 %len15
  %copylen211 = add i64 %len26, 1
  %7 = call ptr @memcpy(ptr %dest210, ptr @str.const.129, i64 %copylen211)
  %len112 = call i64 @strlen(ptr %concat.buf9)
  %len213 = call i64 @strlen(ptr %alloca4)
  %totallen14 = add i64 %len112, %len213
  %bufsize15 = add i64 %totallen14, 1
  %concat.buf16 = alloca i8, i64 %bufsize15, align 1
  %8 = call ptr @memcpy(ptr %concat.buf16, ptr %concat.buf9, i64 %len112)
  %dest217 = getelementptr i8, ptr %concat.buf16, i64 %len112
  %copylen218 = add i64 %len213, 1
  %9 = call ptr @memcpy(ptr %dest217, ptr %alloca4, i64 %copylen218)
  %calltmp = call i64 @"\D8\B3\D8\AC_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(ptr @str.const.130, ptr %concat.buf16)
  %"%\D9\82\D8\A7\D8\B9\D8\AF\D8\A9.load" = load i64, ptr %alloca, align 8
  %"%\D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A9_\D8\A8.load" = load i64, ptr %alloca1, align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\82\D8\A7\D8\B9\D8\AF\D8\A9.load", %"%\D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A9_\D8\A8.load"
  br i1 %cmpeqtmp, label %then_3, label %merge_4

then_3:                                           ; preds = %entry
  %calltmp19 = call i64 @"\D8\B3\D8\AC_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(ptr @str.const.131, ptr @str.const.132)
  %"%\D9\82\D9\81\D8\B2\D8\A7\D8\AA.load" = load i64, ptr %alloca2, align 8
  %addtmp = add i64 %"%\D9\82\D9\81\D8\B2\D8\A7\D8\AA.load", 1
  store i64 %addtmp, ptr %alloca2, align 8
  br label %merge_4

merge_4:                                          ; preds = %then_3, %entry
  %"%\D9\82\D8\A7\D8\B9\D8\AF\D8\A9.load20" = load i64, ptr %alloca, align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\82\D8\A7\D8\B9\D8\AF\D8\A9.load20", 20
  br i1 %cmpgttmp, label %then_5, label %merge_6

then_5:                                           ; preds = %merge_4
  %calltmp21 = call i64 @"\D8\B3\D8\AC_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(ptr @str.const.133, ptr @str.const.134)
  %"%\D9\82\D9\81\D8\B2\D8\A7\D8\AA.load22" = load i64, ptr %alloca2, align 8
  %addtmp23 = add i64 %"%\D9\82\D9\81\D8\B2\D8\A7\D8\AA.load22", 1
  store i64 %addtmp23, ptr %alloca2, align 8
  br label %merge_6

merge_6:                                          ; preds = %then_5, %merge_4
  %"%\D9\82\D8\A7\D8\B9\D8\AF\D8\A9.load24" = load i64, ptr %alloca, align 8
  %cmplttmp = icmp slt i64 %"%\D9\82\D8\A7\D8\B9\D8\AF\D8\A9.load24", 100
  br i1 %cmplttmp, label %then_7, label %merge_8

then_7:                                           ; preds = %merge_6
  %calltmp25 = call i64 @"\D8\B3\D8\AC_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(ptr @str.const.135, ptr @str.const.136)
  %"%\D9\82\D9\81\D8\B2\D8\A7\D8\AA.load26" = load i64, ptr %alloca2, align 8
  %addtmp27 = add i64 %"%\D9\82\D9\81\D8\B2\D8\A7\D8\AA.load26", 1
  store i64 %addtmp27, ptr %alloca2, align 8
  br label %merge_8

merge_8:                                          ; preds = %then_7, %merge_6
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.137)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca28 = alloca ptr, align 8
  %len129 = call i64 @strlen(ptr @str.const.141)
  %len230 = call i64 @strlen(ptr %alloca28)
  %totallen31 = add i64 %len129, %len230
  %bufsize32 = add i64 %totallen31, 1
  %concat.buf33 = alloca i8, i64 %bufsize32, align 1
  %12 = call ptr @memcpy(ptr %concat.buf33, ptr @str.const.141, i64 %len129)
  %dest234 = getelementptr i8, ptr %concat.buf33, i64 %len129
  %copylen235 = add i64 %len230, 1
  %13 = call ptr @memcpy(ptr %dest234, ptr %alloca28, i64 %copylen235)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf33)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.145)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.149)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.153)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.157)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.161)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp = call i64 @"\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9_\D9\85\D9\83\D8\AF\D8\B3"()
  %calltmp1 = call i64 @"\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9_\D8\B3\D8\AC\D9\84\D8\A7\D8\AA"()
  %calltmp2 = call i64 @"\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9_\D8\AD\D9\84\D9\82\D8\A9"()
  %calltmp3 = call i64 @"\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9_\D8\B4\D8\B1\D9\88\D8\B7"()
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.165)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
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
