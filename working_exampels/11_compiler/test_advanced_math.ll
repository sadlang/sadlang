; ModuleID = 'C:\s\s_language\working_exampels\11_compiler\test_advanced_math.sad'
source_filename = "C:\\s\\s_language\\working_exampels\\11_compiler\\test_advanced_math.sad"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@1 = private unnamed_addr constant [66 x i8] c"=== \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 \D8\AF\D9\88\D8\A7\D9\84 \D8\A7\D9\84\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA \D8\A7\D9\84\D9\85\D8\AA\D9\82\D8\AF\D9\85\D8\A9 ===\00", align 1
@2 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@3 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@4 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@5 = private unnamed_addr constant [17 x i8] c"\D9\84\D9\88\D8\BA(2.718) = \00", align 1
@6 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@7 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@8 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@9 = private unnamed_addr constant [15 x i8] c"\D8\A3\D8\B3\D9\91\D9\8A(1) = \00", align 1
@10 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@11 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@12 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@13 = private unnamed_addr constant [23 x i8] c"\D9\82\D9\8A\D9\91\D8\AF(15, 0, 10) = \00", align 1
@14 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@15 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@16 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@17 = private unnamed_addr constant [12 x i8] c"\D8\A8\D8\A7\D9\8A() = \00", align 1
@18 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@19 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@20 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@21 = private unnamed_addr constant [10 x i8] c"\D8\A5\D9\8A() = \00", align 1
@22 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@23 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@24 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@25 = private unnamed_addr constant [21 x i8] c"\D8\A7\D9\82\D8\AA\D8\B7\D8\A7\D8\B9(3.7) = \00", align 1
@26 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@27 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@28 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@29 = private unnamed_addr constant [19 x i8] c"\D8\A8\D8\A7\D9\82\D9\8A(10, 3) = \00", align 1
@30 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@31 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@32 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@33 = private unnamed_addr constant [18 x i8] c"\D8\A5\D8\B4\D8\A7\D8\B1\D8\A9(-5) = \00", align 1
@34 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@35 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@36 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@37 = private unnamed_addr constant [17 x i8] c"\D8\A5\D8\B4\D8\A7\D8\B1\D8\A9(5) = \00", align 1
@38 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@39 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@40 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@41 = private unnamed_addr constant [68 x i8] c"\E2\9C\93 \D8\AC\D9\85\D9\8A\D8\B9 \D8\AF\D9\88\D8\A7\D9\84 \D8\A7\D9\84\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA \D8\A7\D9\84\D9\85\D8\AA\D9\82\D8\AF\D9\85\D8\A9 \D8\AA\D8\B9\D9\85\D9\84!\00", align 1
@42 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@43 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @0, ptr @1)
  %1 = call i32 (ptr, ...) @printf(ptr @2, ptr @3)
  %2 = call i32 (ptr, ...) @printf(ptr @4, ptr @5)
  %3 = call i32 (ptr, ...) @printf(ptr @6, ptr @7)
  %4 = call i32 (ptr, ...) @printf(ptr @8, ptr @9)
  %5 = call i32 (ptr, ...) @printf(ptr @10, ptr @11)
  %6 = call i32 (ptr, ...) @printf(ptr @12, ptr @13)
  %7 = call i32 (ptr, ...) @printf(ptr @14, ptr @15)
  %8 = call i32 (ptr, ...) @printf(ptr @16, ptr @17)
  %9 = call i32 (ptr, ...) @printf(ptr @18, ptr @19)
  %10 = call i32 (ptr, ...) @printf(ptr @20, ptr @21)
  %11 = call i32 (ptr, ...) @printf(ptr @22, ptr @23)
  %12 = call i32 (ptr, ...) @printf(ptr @24, ptr @25)
  %13 = call i32 (ptr, ...) @printf(ptr @26, ptr @27)
  %14 = call i32 (ptr, ...) @printf(ptr @28, ptr @29)
  %15 = call i32 (ptr, ...) @printf(ptr @30, ptr @31)
  %16 = call i32 (ptr, ...) @printf(ptr @32, ptr @33)
  %17 = call i32 (ptr, ...) @printf(ptr @34, ptr @35)
  %18 = call i32 (ptr, ...) @printf(ptr @36, ptr @37)
  %19 = call i32 (ptr, ...) @printf(ptr @38, ptr @39)
  %20 = call i32 (ptr, ...) @printf(ptr @40, ptr @41)
  %21 = call i32 (ptr, ...) @printf(ptr @42, ptr @43)
  ret i64 0
}

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
