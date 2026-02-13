; ModuleID = 'C:\s\s_language\working_exampels\03_strings\test_strings_simple.sad'
source_filename = "C:\\s\\s_language\\working_exampels\\03_strings\\test_strings_simple.sad"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@1 = private unnamed_addr constant [35 x i8] c"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 \D8\AF\D9\88\D8\A7\D9\84 \D8\A7\D9\84\D9\86\D8\B5\D9\88\D8\B5\00", align 1
@2 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@3 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@4 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@5 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@6 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@7 = private unnamed_addr constant [25 x i8] c"\D8\B7\D9\88\D9\84 \D8\A7\D9\84\D9\86\D8\B5 Hello = \00", align 1
@str.const = private unnamed_addr constant [6 x i8] c"Hello\00", align 1
@8 = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@9 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@10 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@11 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@12 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@13 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@14 = private unnamed_addr constant [23 x i8] c"\D8\AA\D8\AD\D9\88\D9\8A\D9\84 \D9\83\D8\A8\D9\8A\D8\B1 = \00", align 1
@str.const.1 = private unnamed_addr constant [12 x i8] c"hello world\00", align 1
@15 = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@16 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@17 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@18 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@19 = private unnamed_addr constant [23 x i8] c"\D8\AA\D8\AD\D9\88\D9\8A\D9\84 \D8\B5\D8\BA\D9\8A\D8\B1 = \00", align 1
@str.const.2 = private unnamed_addr constant [12 x i8] c"HELLO WORLD\00", align 1
@20 = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@21 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@22 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@23 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@24 = private unnamed_addr constant [6 x i8] c"\D8\AA\D9\85!\00", align 1
@25 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@26 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @0, ptr @1)
  %1 = call i32 (ptr, ...) @printf(ptr @2, ptr @3)
  %2 = call i32 (ptr, ...) @printf(ptr @4, ptr @5)
  %3 = call i32 (ptr, ...) @printf(ptr @6, ptr @7)
  %strlen = call i64 @sad_string_length(ptr @str.const)
  %4 = call i32 (ptr, ...) @printf(ptr @8, i64 %strlen)
  %5 = call i32 (ptr, ...) @printf(ptr @9, ptr @10)
  %6 = call i32 (ptr, ...) @printf(ptr @11, ptr @12)
  %7 = call i32 (ptr, ...) @printf(ptr @13, ptr @14)
  %toupper = call ptr @sad_string_to_upper(ptr @str.const.1)
  %print.load = load i64, ptr %toupper, align 8
  %8 = call i32 (ptr, ...) @printf(ptr @15, i64 %print.load)
  %9 = call i32 (ptr, ...) @printf(ptr @16, ptr @17)
  %10 = call i32 (ptr, ...) @printf(ptr @18, ptr @19)
  %tolower = call ptr @sad_string_to_lower(ptr @str.const.2)
  %print.load1 = load i64, ptr %tolower, align 8
  %11 = call i32 (ptr, ...) @printf(ptr @20, i64 %print.load1)
  %12 = call i32 (ptr, ...) @printf(ptr @21, ptr @22)
  %13 = call i32 (ptr, ...) @printf(ptr @23, ptr @24)
  %14 = call i32 (ptr, ...) @printf(ptr @25, ptr @26)
  ret i64 0
}

declare i32 @printf(ptr, ...)

declare i64 @sad_string_length(ptr)

declare ptr @sad_string_to_upper(ptr)

declare ptr @sad_string_to_lower(ptr)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
