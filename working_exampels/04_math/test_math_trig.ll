; ModuleID = 'C:\s\s_language\working_exampels\04_math\test_math_trig.sad'
source_filename = "C:\\s\\s_language\\working_exampels\\04_math\\test_math_trig.sad"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@1 = private unnamed_addr constant [43 x i8] c"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 \D8\A7\D9\84\D8\AF\D9\88\D8\A7\D9\84 \D8\A7\D9\84\D8\AB\D9\84\D8\A7\D8\AB\D9\8A\D8\A9\00", align 1
@2 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@3 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@4 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@5 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@6 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@7 = private unnamed_addr constant [22 x i8] c"\D8\AC\D9\8A\D8\A8 30 \D8\AF\D8\B1\D8\AC\D8\A9 = \00", align 1
@8 = private unnamed_addr constant [3 x i8] c"%f\00", align 1
@9 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@10 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@11 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@12 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@13 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@14 = private unnamed_addr constant [31 x i8] c"\D8\AC\D9\8A\D8\A8 \D8\AA\D9\85\D8\A7\D9\85 60 \D8\AF\D8\B1\D8\AC\D8\A9 = \00", align 1
@15 = private unnamed_addr constant [3 x i8] c"%f\00", align 1
@16 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@17 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@18 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@19 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@20 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@21 = private unnamed_addr constant [20 x i8] c"\D8\B8\D9\84 45 \D8\AF\D8\B1\D8\AC\D8\A9 = \00", align 1
@22 = private unnamed_addr constant [3 x i8] c"%f\00", align 1
@23 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@24 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@25 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@26 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@27 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@28 = private unnamed_addr constant [14 x i8] c"\D9\85\D8\B1\D8\A8\D8\B9 5 = \00", align 1
@29 = private unnamed_addr constant [3 x i8] c"%f\00", align 1
@30 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@31 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@32 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@33 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@34 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@35 = private unnamed_addr constant [6 x i8] c"\D8\AA\D9\85!\00", align 1
@36 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@37 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @0, ptr @1)
  %1 = call i32 (ptr, ...) @printf(ptr @2, ptr @3)
  %2 = call i32 (ptr, ...) @printf(ptr @4, ptr @5)
  %3 = call i32 (ptr, ...) @printf(ptr @6, ptr @7)
  %sintmp = call double @llvm.sin.f64(double 5.235990e-01)
  %4 = call i32 (ptr, ...) @printf(ptr @8, double %sintmp)
  %5 = call i32 (ptr, ...) @printf(ptr @9, ptr @10)
  %6 = call i32 (ptr, ...) @printf(ptr @11, ptr @12)
  %7 = call i32 (ptr, ...) @printf(ptr @13, ptr @14)
  %costmp = call double @llvm.cos.f64(double 0x3FF0C152B0A6FC59)
  %8 = call i32 (ptr, ...) @printf(ptr @15, double %costmp)
  %9 = call i32 (ptr, ...) @printf(ptr @16, ptr @17)
  %10 = call i32 (ptr, ...) @printf(ptr @18, ptr @19)
  %11 = call i32 (ptr, ...) @printf(ptr @20, ptr @21)
  %sin.tan = call double @llvm.sin.f64(double 7.853980e-01)
  %cos.tan = call double @llvm.cos.f64(double 7.853980e-01)
  %tantmp = fdiv double %sin.tan, %cos.tan
  %12 = call i32 (ptr, ...) @printf(ptr @22, double %tantmp)
  %13 = call i32 (ptr, ...) @printf(ptr @23, ptr @24)
  %14 = call i32 (ptr, ...) @printf(ptr @25, ptr @26)
  %15 = call i32 (ptr, ...) @printf(ptr @27, ptr @28)
  %powtmp = call double @llvm.pow.f64(double 2.470330e-323, double 2.000000e+00)
  %16 = call i32 (ptr, ...) @printf(ptr @29, double %powtmp)
  %17 = call i32 (ptr, ...) @printf(ptr @30, ptr @31)
  %18 = call i32 (ptr, ...) @printf(ptr @32, ptr @33)
  %19 = call i32 (ptr, ...) @printf(ptr @34, ptr @35)
  %20 = call i32 (ptr, ...) @printf(ptr @36, ptr @37)
  ret i64 0
}

declare i32 @printf(ptr, ...)

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.sin.f64(double) #0

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.cos.f64(double) #0

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.pow.f64(double, double) #0

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}

attributes #0 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
