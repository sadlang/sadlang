; ModuleID = 'working_exampels\14_tested\01_hello_interactive.sad'
source_filename = "working_exampels\\14_tested\\01_hello_interactive.sad"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@1 = private unnamed_addr constant [30 x i8] c"=============================\00", align 1
@2 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@3 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@4 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@5 = private unnamed_addr constant [35 x i8] c"   \D9\85\D8\B1\D8\AD\D8\A8\D8\A7 \D8\A8\D9\83 \D9\81\D9\8A \D9\84\D8\BA\D8\A9 \D8\B5!\00", align 1
@6 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@7 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@8 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@9 = private unnamed_addr constant [30 x i8] c"=============================\00", align 1
@10 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@11 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@12 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@13 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@14 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@15 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@16 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@17 = private unnamed_addr constant [17 x i8] c"\D9\85\D8\A7 \D8\A7\D8\B3\D9\85\D9\83\D8\9F \00", align 1
@18 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@19 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@20 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@21 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@22 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@23 = private unnamed_addr constant [17 x i8] c"\D9\85\D8\B1\D8\AD\D8\A8\D8\A7 \D9\8A\D8\A7 \00", align 1
@24 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@25 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@26 = private unnamed_addr constant [2 x i8] c"!\00", align 1
@27 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@28 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@29 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@30 = private unnamed_addr constant [71 x i8] c"\D8\A7\D9\87\D9\84\D8\A7 \D9\88\D8\B3\D9\87\D9\84\D8\A7 \D8\A8\D9\83 \D9\81\D9\8A \D8\B9\D8\A7\D9\84\D9\85 \D8\A7\D9\84\D8\A8\D8\B1\D9\85\D8\AC\D8\A9 \D8\A8\D8\A7\D9\84\D8\B9\D8\B1\D8\A8\D9\8A\D8\A9\00", align 1
@31 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@32 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1

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
  %input_str = call ptr @sad_llvm_input_cstr()
  %alloca = alloca ptr, align 8
  store ptr %input_str, ptr %alloca, align 8
  %9 = call i32 (ptr, ...) @printf(ptr @18, ptr @19)
  %10 = call i32 (ptr, ...) @printf(ptr @20, ptr @21)
  %11 = call i32 (ptr, ...) @printf(ptr @22, ptr @23)
  %print.load = load ptr, ptr %alloca, align 8
  %12 = call i32 (ptr, ...) @printf(ptr @24, ptr %print.load)
  %13 = call i32 (ptr, ...) @printf(ptr @25, ptr @26)
  %14 = call i32 (ptr, ...) @printf(ptr @27, ptr @28)
  %15 = call i32 (ptr, ...) @printf(ptr @29, ptr @30)
  %16 = call i32 (ptr, ...) @printf(ptr @31, ptr @32)
  ret i64 0
}

declare i32 @printf(ptr, ...)

declare ptr @sad_llvm_input_cstr()

define i32 @main() {
entry:
  %0 = call i32 @SetConsoleOutputCP(i32 65001)
  %1 = call i32 @SetConsoleCP(i32 65001)
  %2 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %3 = trunc i64 %2 to i32
  ret i32 %3
}

declare i32 @SetConsoleOutputCP(i32)

declare i32 @SetConsoleCP(i32)
