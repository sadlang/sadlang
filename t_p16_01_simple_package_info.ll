; ModuleID = '01_simple_package_info'
source_filename = "01_simple_package_info"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [112 x i8] c"\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.1 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.3 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.7 = private unnamed_addr constant [8 x i8] c"  \F0\9F\93\A6 \00", align 1
@str.const.12 = private unnamed_addr constant [5 x i8] c"    \00", align 1
@str.const.15 = private unnamed_addr constant [4 x i8] c" = \00", align 1
@fmt.d = private unnamed_addr constant [6 x i8] c"%lld\0A\00", align 1
@str.const.27 = private unnamed_addr constant [26 x i8] c"\D9\85\D8\AD\D8\AA\D9\88\D9\89 \D9\85\D9\84\D9\81 \D8\B5.toml\00", align 1
@str.const.28 = private unnamed_addr constant [13 x i8] c"  [\D8\AD\D8\B2\D9\85\D8\A9]\00", align 1
@str.const.32 = private unnamed_addr constant [11 x i8] c"\D8\A7\D9\84\D8\A7\D8\B3\D9\85\00", align 1
@str.const.33 = private unnamed_addr constant [13 x i8] c"\D8\AA\D8\B7\D8\A8\D9\8A\D9\82\D9\8A\00", align 1
@str.const.34 = private unnamed_addr constant [15 x i8] c"\D8\A7\D9\84\D8\A5\D8\B5\D8\AF\D8\A7\D8\B1\00", align 1
@str.const.35 = private unnamed_addr constant [6 x i8] c"1.0.0\00", align 1
@str.const.36 = private unnamed_addr constant [11 x i8] c"\D8\A7\D9\84\D9\88\D8\B5\D9\81\00", align 1
@str.const.37 = private unnamed_addr constant [47 x i8] c"\D8\AA\D8\B7\D8\A8\D9\8A\D9\82 \D8\B9\D8\B1\D8\A8\D9\8A \D8\A8\D8\A7\D8\B3\D8\AA\D8\AE\D8\AF\D8\A7\D9\85 \D9\84\D8\BA\D8\A9 \D8\B5\00", align 1
@str.const.38 = private unnamed_addr constant [13 x i8] c"\D8\A7\D9\84\D9\85\D8\A4\D9\84\D9\81\00", align 1
@str.const.39 = private unnamed_addr constant [18 x i8] c"\D8\A3\D8\AD\D9\85\D8\AF \D9\85\D8\AD\D9\85\D8\AF\00", align 1
@str.const.40 = private unnamed_addr constant [13 x i8] c"\D8\A7\D9\84\D8\B1\D8\AE\D8\B5\D8\A9\00", align 1
@str.const.41 = private unnamed_addr constant [4 x i8] c"MIT\00", align 1
@str.const.42 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.46 = private unnamed_addr constant [17 x i8] c"  [\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA]\00", align 1
@str.const.50 = private unnamed_addr constant [15 x i8] c"\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA\00", align 1
@str.const.51 = private unnamed_addr constant [6 x i8] c"2.1.0\00", align 1
@str.const.52 = private unnamed_addr constant [9 x i8] c"\D9\86\D8\B5\D9\88\D8\B5\00", align 1
@str.const.53 = private unnamed_addr constant [6 x i8] c"1.3.5\00", align 1
@str.const.54 = private unnamed_addr constant [9 x i8] c"\D8\B4\D8\A8\D9\83\D8\A9\00", align 1
@str.const.55 = private unnamed_addr constant [6 x i8] c"0.9.2\00", align 1
@str.const.59 = private unnamed_addr constant [3 x i8] c": \00", align 1
@str.const.61 = private unnamed_addr constant [2 x i8] c".\00", align 1
@str.const.69 = private unnamed_addr constant [54 x i8] c"\D8\A7\D9\84\D8\AA\D8\AD\D9\82\D9\82 \D9\85\D9\86 \D8\A7\D9\84\D8\A5\D8\B5\D8\AF\D8\A7\D8\B1\D8\A7\D8\AA \D8\A7\D9\84\D8\AF\D9\84\D8\A7\D9\84\D9\8A\D8\A9\00", align 1
@str.const.73 = private unnamed_addr constant [37 x i8] c"\D9\81\D8\AD\D8\B5 \D8\AA\D9\88\D8\A7\D9\81\D9\82 \D8\A7\D9\84\D8\A5\D8\B5\D8\AF\D8\A7\D8\B1\D8\A7\D8\AA\00", align 1
@str.const.74 = private unnamed_addr constant [61 x i8] c"    \D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA: \D9\85\D8\B7\D9\84\D9\88\D8\A8 2.1.0\D8\8C \D9\85\D8\AA\D9\88\D9\81\D8\B1 2.3.0 \E2\86\92 \00", align 1
@str.const.76 = private unnamed_addr constant [17 x i8] c"\E2\9C\85 \D9\85\D8\AA\D9\88\D8\A7\D9\81\D9\82\00", align 1
@str.const.80 = private unnamed_addr constant [24 x i8] c"\E2\9D\8C \D8\BA\D9\8A\D8\B1 \D9\85\D8\AA\D9\88\D8\A7\D9\81\D9\82\00", align 1
@str.const.84 = private unnamed_addr constant [55 x i8] c"    \D9\86\D8\B5\D9\88\D8\B5: \D9\85\D8\B7\D9\84\D9\88\D8\A8 1.3.5\D8\8C \D9\85\D8\AA\D9\88\D9\81\D8\B1 2.0.0 \E2\86\92 \00", align 1
@str.const.90 = private unnamed_addr constant [59 x i8] c"\E2\9D\8C \D8\BA\D9\8A\D8\B1 \D9\85\D8\AA\D9\88\D8\A7\D9\81\D9\82 (\D8\A5\D8\B5\D8\AF\D8\A7\D8\B1 \D8\B1\D8\A6\D9\8A\D8\B3\D9\8A \D9\85\D8\AE\D8\AA\D9\84\D9\81)\00", align 1
@str.const.94 = private unnamed_addr constant [56 x i8] c"    \D8\B4\D8\A8\D9\83\D8\A9: \D9\85\D8\B7\D9\84\D9\88\D8\A8 0.9.2\D8\8C \D9\85\D8\AA\D9\88\D9\81\D8\B1 0.9.10 \E2\86\92 \00", align 1
@str.const.104 = private unnamed_addr constant [121 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.108 = private unnamed_addr constant [63 x i8] c"\E2\95\91  \F0\9F\93\A6 \D9\86\D8\B8\D8\A7\D9\85 \D8\A7\D9\84\D8\AD\D8\B2\D9\85 \D8\B5.toml - \D9\85\D8\B9\D9\84\D9\88\D9\85\D8\A7\D8\AA     \E2\95\91\00", align 1
@str.const.112 = private unnamed_addr constant [121 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.116 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.120 = private unnamed_addr constant [57 x i8] c"  \E2\9C\85 \D8\AA\D9\85 \D8\B9\D8\B1\D8\B6 \D9\85\D8\B9\D9\84\D9\88\D9\85\D8\A7\D8\AA \D8\A7\D9\84\D8\AD\D8\B2\D9\85\D8\A9 \D8\A8\D9\86\D8\AC\D8\A7\D8\AD\00", align 1

define i64 @"\D8\AE\D8\B7"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86"(ptr %"\D9\86\D8\B5_\D8\B9\D9\86\D9\88\D8\A7\D9\86") {
entry:
  %"\D9\86\D8\B5_\D8\B9\D9\86\D9\88\D8\A7\D9\861" = alloca ptr, align 8
  store ptr %"\D9\86\D8\B5_\D8\B9\D9\86\D9\88\D8\A7\D9\86", ptr %"\D9\86\D8\B5_\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call i64 @"\D8\AE\D8\B7"()
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D9\86\D8\B5_\D8\B9\D9\86\D9\88\D8\A7\D9\861")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp2 = call i64 @"\D8\AE\D8\B7"()
  ret i64 0
}

define i64 @"\D8\AD\D9\82\D9\84"(ptr %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD", ptr %"\D9\82\D9\8A\D9\85\D8\A9") {
entry:
  %"\D9\82\D9\8A\D9\85\D8\A92" = alloca ptr, align 8
  store ptr %"\D9\82\D9\8A\D9\85\D8\A9", ptr %"\D9\82\D9\8A\D9\85\D8\A92", align 8
  %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD1" = alloca ptr, align 8
  store ptr %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD", ptr %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD1", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD1")
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.15)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D9\82\D9\8A\D9\85\D8\A92")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\AD\D9\82\D9\84_\D8\B1\D9\82\D9\85"(ptr %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD", i64 %"\D9\82\D9\8A\D9\85\D8\A9") {
entry:
  %"\D9\82\D9\8A\D9\85\D8\A92" = alloca i64, align 8
  store i64 %"\D9\82\D9\8A\D9\85\D8\A9", ptr %"\D9\82\D9\8A\D9\85\D8\A92", align 8
  %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD1" = alloca ptr, align 8
  store ptr %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD", ptr %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD1", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD1")
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.15)
  %"%\D9\82\D9\8A\D9\85\D8\A9.load" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A92", align 8
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\B9\D8\B1\D8\B6_\D8\AD\D8\B2\D9\85\D8\A9"() {
entry:
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86"(ptr @str.const.27)
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.28)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp1 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.32, ptr @str.const.33)
  %calltmp2 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.34, ptr @str.const.35)
  %calltmp3 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.36, ptr @str.const.37)
  %calltmp4 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.38, ptr @str.const.39)
  %calltmp5 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.40, ptr @str.const.41)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.42)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.46)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp6 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.50, ptr @str.const.51)
  %calltmp7 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.52, ptr @str.const.53)
  %calltmp8 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.54, ptr @str.const.55)
  ret i64 0
}

define i64 @"\D8\A7\D8\B3\D8\AA\D8\AE\D8\B1\D8\AC_\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A"(i64 %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1") {
entry:
  %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B11" = alloca i64, align 8
  store i64 %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1", ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B11", align 8
  %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load" = load i64, ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B11", align 8
  %divtmp = sdiv i64 %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load", 10000
  ret i64 %divtmp
}

define i64 @"\D8\A7\D8\B3\D8\AA\D8\AE\D8\B1\D8\AC_\D9\81\D8\B1\D8\B9\D9\8A"(i64 %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1") {
entry:
  %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B11" = alloca i64, align 8
  store i64 %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1", ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B11", align 8
  %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load" = load i64, ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B11", align 8
  %divtmp = sdiv i64 %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load", 100
  %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load2" = load i64, ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B11", align 8
  %calltmp = call i64 @"\D8\A7\D8\B3\D8\AA\D8\AE\D8\B1\D8\AC_\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A"(i64 %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load2")
  %multmp = mul i64 %calltmp, 100
  %subtmp = sub i64 %divtmp, %multmp
  ret i64 %subtmp
}

define i64 @"\D8\A7\D8\B3\D8\AA\D8\AE\D8\B1\D8\AC_\D8\AA\D8\B5\D8\AD\D9\8A\D8\AD"(i64 %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1") {
entry:
  %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B11" = alloca i64, align 8
  store i64 %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1", ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B11", align 8
  %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load" = load i64, ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B11", align 8
  %divtmp = sdiv i64 %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load", 100
  %multmp = mul i64 %divtmp, 100
  %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load2" = load i64, ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B11", align 8
  %subtmp = sub i64 %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load2", %multmp
  ret i64 %subtmp
}

define i64 @"\D8\B9\D8\B1\D8\B6_\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1"(ptr %"\D8\AD\D8\B2\D9\85\D8\A9", i64 %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1") {
entry:
  %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B12" = alloca i64, align 8
  store i64 %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1", ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B12", align 8
  %"\D8\AD\D8\B2\D9\85\D8\A91" = alloca ptr, align 8
  store ptr %"\D8\AD\D8\B2\D9\85\D8\A9", ptr %"\D8\AD\D8\B2\D9\85\D8\A91", align 8
  %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load" = load i64, ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B12", align 8
  %calltmp = call i64 @"\D8\A7\D8\B3\D8\AA\D8\AE\D8\B1\D8\AC_\D9\81\D8\B1\D8\B9\D9\8A"(i64 %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load")
  %alloca = alloca i64, align 8
  store i64 %calltmp, ptr %alloca, align 8
  %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load3" = load i64, ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B12", align 8
  %calltmp4 = call i64 @"\D8\A7\D8\B3\D8\AA\D8\AE\D8\B1\D8\AC_\D8\AA\D8\B5\D8\AD\D9\8A\D8\AD"(i64 %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load3")
  %alloca5 = alloca i64, align 8
  store i64 %calltmp4, ptr %alloca5, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D8\AD\D8\B2\D9\85\D8\A91")
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.59)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.61)
  %"%\D9\81\D8\B1\D8\B9\D9\8A.load" = load i64, ptr %alloca, align 8
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D9\81\D8\B1\D8\B9\D9\8A.load")
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.61)
  %"%\D8\AA\D8\B5\D8\AD\D9\8A\D8\AD.load" = load i64, ptr %alloca5, align 8
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\AA\D8\B5\D8\AD\D9\8A\D8\AD.load")
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\AA\D8\AD\D9\82\D9\82_\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1\D8\A7\D8\AA"() {
entry:
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86"(ptr @str.const.69)
  %alloca = alloca i64, align 8
  store i64 20100, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 10305, ptr %alloca1, align 8
  %alloca2 = alloca i64, align 8
  store i64 902, ptr %alloca2, align 8
  %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load" = load i64, ptr %alloca, align 8
  %calltmp3 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1"(ptr @str.const.50, i64 %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load")
  %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D9\86\D8\B5\D9\88\D8\B5.load" = load i64, ptr %alloca1, align 8
  %calltmp4 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1"(ptr @str.const.52, i64 %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D9\86\D8\B5\D9\88\D8\B5.load")
  %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D8\B4\D8\A8\D9\83\D8\A9.load" = load i64, ptr %alloca2, align 8
  %calltmp5 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1"(ptr @str.const.54, i64 %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D8\B4\D8\A8\D9\83\D8\A9.load")
  ret i64 0
}

define i64 @"\D9\85\D8\AA\D9\88\D8\A7\D9\81\D9\82"(i64 %"\D9\85\D8\B7\D9\84\D9\88\D8\A8", i64 %"\D9\85\D8\AA\D9\88\D9\81\D8\B1") {
entry:
  %"\D9\85\D8\AA\D9\88\D9\81\D8\B12" = alloca i64, align 8
  store i64 %"\D9\85\D8\AA\D9\88\D9\81\D8\B1", ptr %"\D9\85\D8\AA\D9\88\D9\81\D8\B12", align 8
  %"\D9\85\D8\B7\D9\84\D9\88\D8\A81" = alloca i64, align 8
  store i64 %"\D9\85\D8\B7\D9\84\D9\88\D8\A8", ptr %"\D9\85\D8\B7\D9\84\D9\88\D8\A81", align 8
  %"%\D9\85\D8\B7\D9\84\D9\88\D8\A8.load" = load i64, ptr %"\D9\85\D8\B7\D9\84\D9\88\D8\A81", align 8
  %calltmp = call i64 @"\D8\A7\D8\B3\D8\AA\D8\AE\D8\B1\D8\AC_\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A"(i64 %"%\D9\85\D8\B7\D9\84\D9\88\D8\A8.load")
  %alloca = alloca i64, align 8
  store i64 %calltmp, ptr %alloca, align 8
  %"%\D9\85\D8\AA\D9\88\D9\81\D8\B1.load" = load i64, ptr %"\D9\85\D8\AA\D9\88\D9\81\D8\B12", align 8
  %calltmp3 = call i64 @"\D8\A7\D8\B3\D8\AA\D8\AE\D8\B1\D8\AC_\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A"(i64 %"%\D9\85\D8\AA\D9\88\D9\81\D8\B1.load")
  %alloca4 = alloca i64, align 8
  store i64 %calltmp3, ptr %alloca4, align 8
  %alloca5 = alloca i64, align 8
  store i64 0, ptr %alloca5, align 8
  %"%\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A_\D9\85\D8\B7\D9\84\D9\88\D8\A8.load" = load i64, ptr %alloca, align 8
  %"%\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A_\D9\85\D8\AA\D9\88\D9\81\D8\B1.load" = load i64, ptr %alloca4, align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A_\D9\85\D8\B7\D9\84\D9\88\D8\A8.load", %"%\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A_\D9\85\D8\AA\D9\88\D9\81\D8\B1.load"
  br i1 %cmpeqtmp, label %then_0, label %merge_1

then_0:                                           ; preds = %entry
  %"%\D9\85\D8\AA\D9\88\D9\81\D8\B1.load6" = load i64, ptr %"\D9\85\D8\AA\D9\88\D9\81\D8\B12", align 8
  %"%\D9\85\D8\B7\D9\84\D9\88\D8\A8.load7" = load i64, ptr %"\D9\85\D8\B7\D9\84\D9\88\D8\A81", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\85\D8\AA\D9\88\D9\81\D8\B1.load6", %"%\D9\85\D8\B7\D9\84\D9\88\D8\A8.load7"
  br i1 %cmpgetmp, label %then_2, label %merge_3

merge_1:                                          ; preds = %merge_3, %entry
  %loadtmp = load i64, ptr %alloca5, align 8
  ret i64 %loadtmp

then_2:                                           ; preds = %then_0
  store i64 1, ptr %alloca5, align 8
  br label %merge_3

merge_3:                                          ; preds = %then_2, %then_0
  br label %merge_1
}

define i64 @"\D9\81\D8\AD\D8\B5_\D8\AA\D9\88\D8\A7\D9\81\D9\82"() {
entry:
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86"(ptr @str.const.73)
  %alloca = alloca i64, align 8
  store i64 20100, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 20300, ptr %alloca1, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.74)
  %"%\D9\85\D8\B7\D9\84\D9\88\D8\A81.load" = load i64, ptr %alloca, align 8
  %"%\D9\85\D8\AA\D9\88\D9\81\D8\B11.load" = load i64, ptr %alloca1, align 8
  %calltmp2 = call i64 @"\D9\85\D8\AA\D9\88\D8\A7\D9\81\D9\82"(i64 %"%\D9\85\D8\B7\D9\84\D9\88\D8\A81.load", i64 %"%\D9\85\D8\AA\D9\88\D9\81\D8\B11.load")
  %cmpeqtmp = icmp eq i64 %calltmp2, 1
  br i1 %cmpeqtmp, label %then_4, label %else_6

then_4:                                           ; preds = %entry
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.76)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_5

else_6:                                           ; preds = %entry
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.80)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_5

merge_5:                                          ; preds = %else_6, %then_4
  %alloca3 = alloca i64, align 8
  store i64 10305, ptr %alloca3, align 8
  %alloca4 = alloca i64, align 8
  store i64 20000, ptr %alloca4, align 8
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.84)
  %"%\D9\85\D8\B7\D9\84\D9\88\D8\A82.load" = load i64, ptr %alloca3, align 8
  %"%\D9\85\D8\AA\D9\88\D9\81\D8\B12.load" = load i64, ptr %alloca4, align 8
  %calltmp5 = call i64 @"\D9\85\D8\AA\D9\88\D8\A7\D9\81\D9\82"(i64 %"%\D9\85\D8\B7\D9\84\D9\88\D8\A82.load", i64 %"%\D9\85\D8\AA\D9\88\D9\81\D8\B12.load")
  %cmpeqtmp6 = icmp eq i64 %calltmp5, 1
  br i1 %cmpeqtmp6, label %then_7, label %else_9

then_7:                                           ; preds = %merge_5
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.76)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_8

else_9:                                           ; preds = %merge_5
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.90)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_8

merge_8:                                          ; preds = %else_9, %then_7
  %alloca7 = alloca i64, align 8
  store i64 902, ptr %alloca7, align 8
  %alloca8 = alloca i64, align 8
  store i64 910, ptr %alloca8, align 8
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.94)
  %"%\D9\85\D8\B7\D9\84\D9\88\D8\A83.load" = load i64, ptr %alloca7, align 8
  %"%\D9\85\D8\AA\D9\88\D9\81\D8\B13.load" = load i64, ptr %alloca8, align 8
  %calltmp9 = call i64 @"\D9\85\D8\AA\D9\88\D8\A7\D9\81\D9\82"(i64 %"%\D9\85\D8\B7\D9\84\D9\88\D8\A83.load", i64 %"%\D9\85\D8\AA\D9\88\D9\81\D8\B13.load")
  %cmpeqtmp10 = icmp eq i64 %calltmp9, 1
  br i1 %cmpeqtmp10, label %then_10, label %else_12

then_10:                                          ; preds = %merge_8
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.76)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_11

else_12:                                          ; preds = %merge_8
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.80)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_11

merge_11:                                         ; preds = %else_12, %then_10
  ret i64 0
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.104)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.108)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.112)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AD\D8\B2\D9\85\D8\A9"()
  %calltmp1 = call i64 @"\D8\AA\D8\AD\D9\82\D9\82_\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1\D8\A7\D8\AA"()
  %calltmp2 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\AA\D9\88\D8\A7\D9\81\D9\82"()
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.116)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp3 = call i64 @"\D8\AE\D8\B7"()
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.120)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp4 = call i64 @"\D8\AE\D8\B7"()
  ret i64 0
}

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
