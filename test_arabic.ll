; ModuleID = 'test_arabic.sad'
source_filename = "test_arabic.sad"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [33 x i8] c"\D9\85\D8\B1\D8\AD\D8\A8\D8\A7 \D9\85\D9\86 \D9\85\D9\84\D9\81 \D8\B9\D8\B1\D8\A8\D9\8A!\00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  ret i64 0
}

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
