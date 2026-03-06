; ModuleID = 'test_stdlib_sadc'
source_filename = "test_stdlib_sadc"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@"\D9\86" = global i64 0
@"\D8\B9" = global i64 0
@"\D8\B5" = global i64 0
@"\D8\AC" = global i64 0
@"\D9\82" = global i64 0
@"\D9\85" = global i64 0
@"\D9\83\D8\A8\D9\8A\D8\B1" = global i64 0
@"\D8\B5\D8\BA\D9\8A\D8\B1" = global i64 0
@"\D8\AA\D9\82" = global i64 0
@"\D8\A3\D8\B1" = global i64 0
@"\D8\B3\D9\82" = global i64 0
@"\D8\B31" = global i64 0
@"\D8\B32" = global i64 0
@"\D9\86\D8\B51" = global i64 0
@"\D8\B7" = global i64 0
@"\D9\86\D8\B52" = global i64 0
@"\D9\81" = global i64 0
@"\D8\B1" = global i64 42
@"\D9\87\D8\B1" = global i64 0
@"\D9\851" = global i64 0
@"\D9\852" = global i64 0
@"\D8\AA" = global i64 0
@"\D8\A81" = global i64 0
@"\D8\A82" = global i64 0
@"\D8\A83" = global i64 0
@"\D9\85\D8\B1\D8\A8" = global i64 0
@str.const = private unnamed_addr constant [21 x i8] c"=== 1 \D8\B7\D8\A8\D8\A7\D8\B9\D8\A9 ===\00", align 1
@fmt.s = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@str.const.1 = private unnamed_addr constant [12 x i8] c"\D9\85\D8\B1\D8\AD\D8\A8\D8\A7 \00", align 1
@str.const.3 = private unnamed_addr constant [9 x i8] c"\D8\B9\D8\A7\D9\84\D9\85\00", align 1
@str.const.5 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.7 = private unnamed_addr constant [21 x i8] c"=== 2 \D8\AA\D8\AD\D9\88\D9\8A\D9\84 ===\00", align 1
@str.const.9 = private unnamed_addr constant [3 x i8] c"42\00", align 1
@str.const.10 = private unnamed_addr constant [10 x i8] c"\D9\84\D8\B1\D9\82\D9\85=\00", align 1
@fmt.d = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@str.const.12 = private unnamed_addr constant [5 x i8] c"3.14\00", align 1
@str.const.13 = private unnamed_addr constant [13 x i8] c" \D9\84\D8\B9\D8\B4\D8\B1\D9\8A=\00", align 1
@fmt.f = private unnamed_addr constant [3 x i8] c"%f\00", align 1
@str.const.15 = private unnamed_addr constant [9 x i8] c" \D9\84\D9\86\D8\B5=\00", align 1
@str.const.20 = private unnamed_addr constant [25 x i8] c"=== 3 \D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA ===\00", align 1
@str.const.22 = private unnamed_addr constant [13 x i8] c"\D8\AC\D8\B0\D8\B1(144)=\00", align 1
@str.const.25 = private unnamed_addr constant [13 x i8] c" \D8\A3\D8\B3(2,10)=\00", align 1
@str.const.28 = private unnamed_addr constant [16 x i8] c" \D9\85\D8\B7\D9\84\D9\82(-42)=\00", align 1
@str.const.33 = private unnamed_addr constant [28 x i8] c"=== 4 \D8\A3\D9\83\D8\A8\D8\B1/\D8\A3\D8\B5\D8\BA\D8\B1 ===\00", align 1
@str.const.35 = private unnamed_addr constant [17 x i8] c"\D8\A3\D9\83\D8\A8\D8\B1(10,20)=\00", align 1
@str.const.38 = private unnamed_addr constant [18 x i8] c" \D8\A3\D8\B5\D8\BA\D8\B1(10,20)=\00", align 1
@str.const.43 = private unnamed_addr constant [21 x i8] c"=== 5 \D8\AA\D9\82\D8\B1\D9\8A\D8\A8 ===\00", align 1
@str.const.45 = private unnamed_addr constant [17 x i8] c"\D8\AA\D9\82\D8\B1\D9\8A\D8\A8(3.7)=\00", align 1
@str.const.48 = private unnamed_addr constant [18 x i8] c" \D8\A3\D8\B1\D8\B6\D9\8A\D8\A9(3.9)=\00", align 1
@str.const.51 = private unnamed_addr constant [14 x i8] c" \D8\B3\D9\82\D9\81(3.1)=\00", align 1
@str.const.56 = private unnamed_addr constant [23 x i8] c"=== 6 \D9\85\D8\AB\D9\84\D8\AB\D8\A7\D8\AA ===\00", align 1
@str.const.58 = private unnamed_addr constant [11 x i8] c"\D8\AC\D9\8A\D8\A8(0)=\00", align 1
@str.const.61 = private unnamed_addr constant [12 x i8] c" \D8\AC\D8\AA\D8\A7(0)=\00", align 1
@str.const.66 = private unnamed_addr constant [19 x i8] c"=== 7 \D9\86\D8\B5\D9\88\D8\B5 ===\00", align 1
@str.const.68 = private unnamed_addr constant [25 x i8] c"\D9\85\D8\B1\D8\AD\D8\A8\D8\A7 \D9\8A\D8\A7 \D8\B9\D8\A7\D9\84\D9\85\00", align 1
@str.const.69 = private unnamed_addr constant [8 x i8] c"\D8\B7\D9\88\D9\84=\00", align 1
@str.const.73 = private unnamed_addr constant [9 x i8] c"\D8\AF\D9\86\D9\8A\D8\A7\00", align 1
@str.const.74 = private unnamed_addr constant [15 x i8] c" \D8\A7\D8\B3\D8\AA\D8\A8\D8\AF\D9\84=\00", align 1
@str.const.78 = private unnamed_addr constant [9 x i8] c" \D8\A8\D8\AD\D8\AB=\00", align 1
@str.const.83 = private unnamed_addr constant [21 x i8] c"=== 8 \D8\A3\D9\86\D9\88\D8\A7\D8\B9 ===\00", align 1
@str.const.85 = private unnamed_addr constant [17 x i8] c"\D9\87\D9\88_\D8\B1\D9\82\D9\85(42)=\00", align 1
@str.const.90 = private unnamed_addr constant [21 x i8] c"=== 9 \D9\85\D9\86\D8\B7\D9\82\D9\8A ===\00", align 1
@str.const.92 = private unnamed_addr constant [17 x i8] c"\D9\84\D9\85\D9\86\D8\B7\D9\82\D9\8A(1)=\00", align 1
@str.const.95 = private unnamed_addr constant [18 x i8] c" \D9\84\D9\85\D9\86\D8\B7\D9\82\D9\8A(0)=\00", align 1
@str.const.100 = private unnamed_addr constant [22 x i8] c"=== 10 \D8\A7\D9\84\D9\86\D9\88\D8\B9 ===\00", align 1
@typeof_str = private unnamed_addr constant [39 x i8] c"\E2\95\AA\E2\95\A3\E2\95\AA\C2\BB\E2\95\AA\C2\BB_\E2\95\AA\E2\95\A1\E2\95\AA\C2\A1\E2\94\98\D7\9A\E2\95\AA\C2\A1\00", align 1
@str.const.102 = private unnamed_addr constant [16 x i8] c"\D8\A7\D9\84\D9\86\D9\88\D8\B9(42)=\00", align 1
@str.const.107 = private unnamed_addr constant [23 x i8] c"=== 11 \D9\86\D8\B5_\D9\81\D8\AD\D8\B5 ===\00", align 1
@str.const.109 = private unnamed_addr constant [11 x i8] c"\D9\85\D8\B1\D8\AD\D8\A8\D8\A7\00", align 1
@str.const.110 = private unnamed_addr constant [5 x i8] c"\D9\85\D8\B1\00", align 1
@str.const.111 = private unnamed_addr constant [13 x i8] c"\D9\8A\D8\A8\D8\AF\D8\A3_\D8\A8=\00", align 1
@str.const.115 = private unnamed_addr constant [5 x i8] c"\D8\A8\D8\A7\00", align 1
@str.const.116 = private unnamed_addr constant [16 x i8] c" \D9\8A\D9\86\D8\AA\D9\87\D9\8A_\D8\A8=\00", align 1
@str.const.120 = private unnamed_addr constant [5 x i8] c"\D9\8A\D8\A7\00", align 1
@str.const.121 = private unnamed_addr constant [13 x i8] c" \D9\8A\D8\AD\D8\AA\D9\88\D9\8A=\00", align 1
@str.const.126 = private unnamed_addr constant [20 x i8] c"=== 12 \D9\85\D8\B1\D8\A8\D8\B9 ===\00", align 1
@str.const.128 = private unnamed_addr constant [13 x i8] c"\D9\85\D8\B1\D8\A8\D8\B9(7)=\00", align 1
@str.const.133 = private unnamed_addr constant [54 x i8] c"=== \D9\83\D9\84 \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA \D8\A7\D9\84\D9\85\D9\83\D8\AA\D8\A8\D8\A9 \D9\86\D8\AC\D8\AD\D8\AA ===\00", align 1

define void @__sad_main() {
entry:
  %"%\D9\85\D8\B1\D8\A8" = alloca double, align 8
  %"%\D8\A83" = alloca i64, align 8
  %"%\D8\A82" = alloca i64, align 8
  %"%\D8\A81" = alloca i64, align 8
  %"%\D8\AA" = alloca ptr, align 8
  %"%\D9\852" = alloca i64, align 8
  %"%\D9\851" = alloca i64, align 8
  %"%\D9\87\D8\B1" = alloca i64, align 8
  %"%\D8\B1" = alloca i64, align 8
  %"%\D9\81" = alloca i64, align 8
  %"%\D9\86\D8\B52" = alloca ptr, align 8
  %"%\D8\B7" = alloca i64, align 8
  %"%\D9\86\D8\B51" = alloca ptr, align 8
  %"%\D8\B32" = alloca double, align 8
  %"%\D8\B31" = alloca double, align 8
  %"%\D8\B3\D9\82" = alloca i64, align 8
  %"%\D8\A3\D8\B1" = alloca i64, align 8
  %"%\D8\AA\D9\82" = alloca i64, align 8
  %"%\D8\B5\D8\BA\D9\8A\D8\B1" = alloca i64, align 8
  %"%\D9\83\D8\A8\D9\8A\D8\B1" = alloca i64, align 8
  %"%\D9\85" = alloca i64, align 8
  %"%\D9\82" = alloca double, align 8
  %"%\D8\AC" = alloca double, align 8
  %"%\D8\B5" = alloca ptr, align 8
  %"%\D8\B9" = alloca double, align 8
  %"%\D9\86" = alloca i64, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %str2i64 = call i64 @atoll(ptr @str.const.9)
  store i64 %str2i64, ptr %"%\D9\86", align 8
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.10)
  %"%\D9\86.load" = load i64, ptr %"%\D9\86", align 8
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D9\86.load")
  %str2f64 = call double @atof(ptr @str.const.12)
  store double %str2f64, ptr %"%\D8\B9", align 8
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.13)
  %"%\D8\B9.load" = load double, ptr %"%\D8\B9", align 8
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.f, double %"%\D8\B9.load")
  %i64str_buf = call ptr @malloc(i64 32)
  %9 = call i32 (ptr, ptr, ...) @sprintf(ptr %i64str_buf, ptr @fmt.d, i64 100)
  store ptr %i64str_buf, ptr %"%\D8\B5", align 8
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.15)
  %"%\D8\B5.load" = load ptr, ptr %"%\D8\B5", align 8
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"%\D8\B5.load")
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.20)
  %sqrt.ret = call double @sqrt(double 1.440000e+02)
  store double %sqrt.ret, ptr %"%\D8\AC", align 8
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.22)
  %"%\D8\AC.load" = load double, ptr %"%\D8\AC", align 8
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.f, double %"%\D8\AC.load")
  %pow.ret = call double @pow(double 2.000000e+00, double 1.000000e+01)
  store double %pow.ret, ptr %"%\D9\82", align 8
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.25)
  %"%\D9\82.load" = load double, ptr %"%\D9\82", align 8
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.f, double %"%\D9\82.load")
  %fabs.ret = call double @fabs(double -4.200000e+01)
  %18 = fptosi double %fabs.ret to i64
  store i64 %18, ptr %"%\D9\85", align 8
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.28)
  %"%\D9\85.load" = load i64, ptr %"%\D9\85", align 8
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D9\85.load")
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.33)
  store i64 20, ptr %"%\D9\83\D8\A8\D9\8A\D8\B1", align 8
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.35)
  %"%\D9\83\D8\A8\D9\8A\D8\B1.load" = load i64, ptr %"%\D9\83\D8\A8\D9\8A\D8\B1", align 8
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D9\83\D8\A8\D9\8A\D8\B1.load")
  store i64 10, ptr %"%\D8\B5\D8\BA\D9\8A\D8\B1", align 8
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.38)
  %"%\D8\B5\D8\BA\D9\8A\D8\B1.load" = load i64, ptr %"%\D8\B5\D8\BA\D9\8A\D8\B1", align 8
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\B5\D8\BA\D9\8A\D8\B1.load")
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.43)
  %round.ret = call double @round(double 3.700000e+00)
  %29 = fptosi double %round.ret to i64
  store i64 %29, ptr %"%\D8\AA\D9\82", align 8
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.45)
  %"%\D8\AA\D9\82.load" = load i64, ptr %"%\D8\AA\D9\82", align 8
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\AA\D9\82.load")
  %floor.ret = call double @floor(double 3.900000e+00)
  %32 = fptosi double %floor.ret to i64
  store i64 %32, ptr %"%\D8\A3\D8\B1", align 8
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.48)
  %"%\D8\A3\D8\B1.load" = load i64, ptr %"%\D8\A3\D8\B1", align 8
  %34 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A3\D8\B1.load")
  %ceil.ret = call double @ceil(double 3.100000e+00)
  %35 = fptosi double %ceil.ret to i64
  store i64 %35, ptr %"%\D8\B3\D9\82", align 8
  %36 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.51)
  %"%\D8\B3\D9\82.load" = load i64, ptr %"%\D8\B3\D9\82", align 8
  %37 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\B3\D9\82.load")
  %38 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %39 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.56)
  %sin.ret = call double @sin(double 0.000000e+00)
  store double %sin.ret, ptr %"%\D8\B31", align 8
  %40 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.58)
  %"%\D8\B31.load" = load double, ptr %"%\D8\B31", align 8
  %41 = call i32 (ptr, ...) @printf(ptr @fmt.f, double %"%\D8\B31.load")
  %cos.ret = call double @cos(double 0.000000e+00)
  store double %cos.ret, ptr %"%\D8\B32", align 8
  %42 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.61)
  %"%\D8\B32.load" = load double, ptr %"%\D8\B32", align 8
  %43 = call i32 (ptr, ...) @printf(ptr @fmt.f, double %"%\D8\B32.load")
  %44 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %45 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.66)
  store ptr @str.const.68, ptr %"%\D9\86\D8\B51", align 8
  %"%\D9\86\D8\B51.load" = load ptr, ptr %"%\D9\86\D8\B51", align 8
  %strlen.ret = call i64 @strlen(ptr %"%\D9\86\D8\B51.load")
  store i64 %strlen.ret, ptr %"%\D8\B7", align 8
  %46 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.69)
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %47 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\B7.load")
  %"%\D9\86\D8\B51.load1" = load ptr, ptr %"%\D9\86\D8\B51", align 8
  %src.len = call i64 @strlen(ptr %"%\D9\86\D8\B51.load1")
  %old.len = call i64 @strlen(ptr @str.const.3)
  %new.len = call i64 @strlen(ptr @str.const.73)
  %48 = mul i64 %src.len, 2
  %49 = add i64 %48, %new.len
  %50 = add i64 %49, 1
  %replace_buf = call ptr @malloc(i64 %50)
  %found = call ptr @strstr(ptr %"%\D9\86\D8\B51.load1", ptr @str.const.3)
  %isnull = icmp eq ptr %found, null
  br i1 %isnull, label %replace.notfound, label %replace.found

replace.found:                                    ; preds = %entry
  %51 = ptrtoint ptr %found to i64
  %52 = ptrtoint ptr %"%\D9\86\D8\B51.load1" to i64
  %53 = sub i64 %51, %52
  %prefix.len = sdiv exact i64 %53, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %54 = call ptr @memcpy(ptr %replace_buf, ptr %"%\D9\86\D8\B51.load1", i64 %prefix.len)
  %dst1 = getelementptr i8, ptr %replace_buf, i64 %prefix.len
  %55 = call ptr @memcpy(ptr %dst1, ptr @str.const.73, i64 %new.len)
  %dst2 = getelementptr i8, ptr %dst1, i64 %new.len
  %suffix.start = getelementptr i8, ptr %found, i64 %old.len
  %56 = add i64 %prefix.len, %old.len
  %57 = sub i64 %src.len, %56
  %58 = add i64 %57, 1
  %59 = call ptr @memcpy(ptr %dst2, ptr %suffix.start, i64 %58)
  br label %replace.merge

replace.notfound:                                 ; preds = %entry
  %60 = call ptr @strcpy(ptr %replace_buf, ptr %"%\D9\86\D8\B51.load1")
  br label %replace.merge

replace.merge:                                    ; preds = %replace.found, %replace.notfound
  store ptr %replace_buf, ptr %"%\D9\86\D8\B52", align 8
  %61 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.74)
  %"%\D9\86\D8\B52.load" = load ptr, ptr %"%\D9\86\D8\B52", align 8
  %62 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"%\D9\86\D8\B52.load")
  %"%\D9\86\D8\B51.load2" = load ptr, ptr %"%\D9\86\D8\B51", align 8
  %found3 = call ptr @strstr(ptr %"%\D9\86\D8\B51.load2", ptr @str.const.3)
  %isnull4 = icmp eq ptr %found3, null
  %found.int = ptrtoint ptr %found3 to i64
  %hstack.int = ptrtoint ptr %"%\D9\86\D8\B51.load2" to i64
  %offset = sub i64 %found.int, %hstack.int
  %find_result = select i1 %isnull4, i64 -1, i64 %offset
  store i64 %find_result, ptr %"%\D9\81", align 8
  %63 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.78)
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %64 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D9\81.load")
  %65 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %66 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.83)
  store i64 42, ptr %"%\D8\B1", align 8
  %"%\D8\B1.load" = load i64, ptr %"%\D8\B1", align 8
  store i64 1, ptr %"%\D9\87\D8\B1", align 8
  %67 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.85)
  %"%\D9\87\D8\B1.load" = load i64, ptr %"%\D9\87\D8\B1", align 8
  %68 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D9\87\D8\B1.load")
  %69 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %70 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.90)
  store i64 1, ptr %"%\D9\851", align 8
  %71 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.92)
  %"%\D9\851.load" = load i64, ptr %"%\D9\851", align 8
  %72 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D9\851.load")
  store i64 0, ptr %"%\D9\852", align 8
  %73 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.95)
  %"%\D9\852.load" = load i64, ptr %"%\D9\852", align 8
  %74 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D9\852.load")
  %75 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %76 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.100)
  store ptr @typeof_str, ptr %"%\D8\AA", align 8
  %77 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.102)
  %"%\D8\AA.load" = load ptr, ptr %"%\D8\AA", align 8
  %78 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"%\D8\AA.load")
  %79 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %80 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.107)
  %pref.len = call i64 @strlen(ptr @str.const.110)
  %starts.cmp = call i32 @strncmp(ptr @str.const.109, ptr @str.const.110, i64 %pref.len)
  %starts_with = icmp eq i32 %starts.cmp, 0
  %starts_with.i64 = zext i1 %starts_with to i64
  store i64 %starts_with.i64, ptr %"%\D8\A81", align 8
  %81 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.111)
  %"%\D8\A81.load" = load i64, ptr %"%\D8\A81", align 8
  %82 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A81.load")
  %str.len = call i64 @strlen(ptr @str.const.109)
  %suf.len = call i64 @strlen(ptr @str.const.115)
  %offset5 = sub i64 %str.len, %suf.len
  %end.ptr = getelementptr i8, ptr @str.const.109, i64 %offset5
  %ends.cmp = call i32 @strcmp(ptr %end.ptr, ptr @str.const.115)
  %ends_with = icmp eq i32 %ends.cmp, 0
  %ends_with.i64 = zext i1 %ends_with to i64
  store i64 %ends_with.i64, ptr %"%\D8\A82", align 8
  %83 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.116)
  %"%\D8\A82.load" = load i64, ptr %"%\D8\A82", align 8
  %84 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A82.load")
  %found6 = call ptr @strstr(ptr @str.const.68, ptr @str.const.120)
  %contains = icmp ne ptr %found6, null
  %contains.i64 = zext i1 %contains to i64
  store i64 %contains.i64, ptr %"%\D8\A83", align 8
  %85 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.121)
  %"%\D8\A83.load" = load i64, ptr %"%\D8\A83", align 8
  %86 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A83.load")
  %87 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %88 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.126)
  %pow.ret7 = call double @pow(double 7.000000e+00, double 2.000000e+00)
  store double %pow.ret7, ptr %"%\D9\85\D8\B1\D8\A8", align 8
  %89 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.128)
  %"%\D9\85\D8\B1\D8\A8.load" = load double, ptr %"%\D9\85\D8\B1\D8\A8", align 8
  %90 = call i32 (ptr, ...) @printf(ptr @fmt.f, double %"%\D9\85\D8\B1\D8\A8.load")
  %91 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %92 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.133)
  %93 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  ret void
}

declare i32 @printf(ptr, ...)

declare i64 @atoll(ptr)

declare double @atof(ptr)

declare ptr @malloc(i64)

declare i32 @sprintf(ptr, ptr, ...)

declare double @sqrt(double)

declare double @pow(double, double)

declare double @fabs(double)

declare double @round(double)

declare double @floor(double)

declare double @ceil(double)

declare double @sin(double)

declare double @cos(double)

declare i64 @strlen(ptr)

declare ptr @strstr(ptr, ptr)

declare ptr @strcpy(ptr, ptr)

declare ptr @memcpy(ptr, ptr, i64)

declare i32 @strncmp(ptr, ptr, i64)

declare i32 @strcmp(ptr, ptr)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
