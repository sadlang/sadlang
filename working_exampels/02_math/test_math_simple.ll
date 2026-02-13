; ModuleID = 'C:\s\s_language\working_exampels\02_math\test_math_simple.sad'
source_filename = "C:\\s\\s_language\\working_exampels\\02_math\\test_math_simple.sad"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@1 = private unnamed_addr constant [44 x i8] c"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 \D8\A7\D9\84\D8\AF\D9\88\D8\A7\D9\84 \D8\A7\D9\84\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A9:\00", align 1
@2 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@3 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@4 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@5 = private unnamed_addr constant [16 x i8] c"\D8\AC\D8\B0\D8\B1(16.0) = \00", align 1
@6 = private unnamed_addr constant [3 x i8] c"%f\00", align 1
@7 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@8 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@9 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@10 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@11 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@12 = private unnamed_addr constant [18 x i8] c"\D8\A3\D8\B3(2.0, 3.0) = \00", align 1
@13 = private unnamed_addr constant [3 x i8] c"%f\00", align 1
@14 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@15 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@16 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@17 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@18 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@19 = private unnamed_addr constant [18 x i8] c"\D9\85\D8\B7\D9\84\D9\82(-5.0) = \00", align 1
@20 = private unnamed_addr constant [3 x i8] c"%f\00", align 1
@21 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@22 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@23 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@24 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@25 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@26 = private unnamed_addr constant [19 x i8] c"\D8\AA\D9\82\D8\B1\D9\8A\D8\A8(3.7) = \00", align 1
@27 = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@28 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@29 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@30 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@31 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@32 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@33 = private unnamed_addr constant [19 x i8] c"\D8\A3\D8\B1\D8\B6\D9\8A\D8\A9(3.9) = \00", align 1
@34 = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@35 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@36 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@37 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@38 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@39 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@40 = private unnamed_addr constant [15 x i8] c"\D8\B3\D9\82\D9\81(3.1) = \00", align 1
@41 = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@42 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@43 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@44 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@45 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @0, ptr @1)
  %1 = call i32 (ptr, ...) @printf(ptr @2, ptr @3)
  %2 = call i32 (ptr, ...) @printf(ptr @4, ptr @5)
  %sqrttmp = call double @llvm.sqrt.f64(double 1.600000e+01)
  %3 = call i32 (ptr, ...) @printf(ptr @6, double %sqrttmp)
  %4 = call i32 (ptr, ...) @printf(ptr @7, ptr @8)
  %5 = call i32 (ptr, ...) @printf(ptr @9, ptr @10)
  %6 = call i32 (ptr, ...) @printf(ptr @11, ptr @12)
  %powtmp = call double @llvm.pow.f64(double 2.000000e+00, double 3.000000e+00)
  %7 = call i32 (ptr, ...) @printf(ptr @13, double %powtmp)
  %8 = call i32 (ptr, ...) @printf(ptr @14, ptr @15)
  %9 = call i32 (ptr, ...) @printf(ptr @16, ptr @17)
  %10 = call i32 (ptr, ...) @printf(ptr @18, ptr @19)
  %abstmp = call double @llvm.fabs.f64(double -5.000000e+00)
  %11 = call i32 (ptr, ...) @printf(ptr @20, double %abstmp)
  %12 = call i32 (ptr, ...) @printf(ptr @21, ptr @22)
  %13 = call i32 (ptr, ...) @printf(ptr @23, ptr @24)
  %14 = call i32 (ptr, ...) @printf(ptr @25, ptr @26)
  %roundtmp = call double @llvm.round.f64(double 3.700000e+00)
  %round.toi64 = fptosi double %roundtmp to i64
  %15 = call i32 (ptr, ...) @printf(ptr @27, i64 %round.toi64)
  %16 = call i32 (ptr, ...) @printf(ptr @28, ptr @29)
  %17 = call i32 (ptr, ...) @printf(ptr @30, ptr @31)
  %18 = call i32 (ptr, ...) @printf(ptr @32, ptr @33)
  %floortmp = call double @llvm.floor.f64(double 3.900000e+00)
  %floor.toi64 = fptosi double %floortmp to i64
  %19 = call i32 (ptr, ...) @printf(ptr @34, i64 %floor.toi64)
  %20 = call i32 (ptr, ...) @printf(ptr @35, ptr @36)
  %21 = call i32 (ptr, ...) @printf(ptr @37, ptr @38)
  %22 = call i32 (ptr, ...) @printf(ptr @39, ptr @40)
  %ceiltmp = call double @llvm.ceil.f64(double 3.100000e+00)
  %ceil.toi64 = fptosi double %ceiltmp to i64
  %23 = call i32 (ptr, ...) @printf(ptr @41, i64 %ceil.toi64)
  %24 = call i32 (ptr, ...) @printf(ptr @42, ptr @43)
  %25 = call i32 (ptr, ...) @printf(ptr @44, ptr @45)
  ret i64 0
}

declare i32 @printf(ptr, ...)

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.sqrt.f64(double) #0

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.pow.f64(double, double) #0

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.fabs.f64(double) #0

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.round.f64(double) #0

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.floor.f64(double) #0

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.ceil.f64(double) #0

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}

attributes #0 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
