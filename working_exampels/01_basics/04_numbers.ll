; ModuleID = 'C:\s\s_language\working_exampels\01_basics\04_numbers.sad'
source_filename = "C:\\s\\s_language\\working_exampels\\01_basics\\04_numbers.sad"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@1 = private unnamed_addr constant [26 x i8] c"\D8\A7\D9\84\D8\B9\D8\AF\D8\AF \D8\A7\D9\84\D8\A3\D9\88\D9\84: 10\00", align 1
@2 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@3 = private unnamed_addr constant [28 x i8] c"\D8\A7\D9\84\D8\B9\D8\AF\D8\AF \D8\A7\D9\84\D8\AB\D8\A7\D9\86\D9\8A: 20\00", align 1
@4 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@5 = private unnamed_addr constant [19 x i8] c"\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9: 30\00", align 1

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @0, ptr @1)
  %1 = call i32 (ptr, ...) @printf(ptr @2, ptr @3)
  %2 = call i32 (ptr, ...) @printf(ptr @4, ptr @5)
  ret i64 0
}

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
