; ModuleID = 'test_oop_mini'
source_filename = "test_oop_mini"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [7 x i8] c"\D9\82\D8\B7\D8\A9\00", align 1
@str.const.1 = private unnamed_addr constant [5 x i8] c"\D8\AA\D9\85\00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.2 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1

define void @"\D8\AD\D9\8A\D9\88\D8\A7\D9\86.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, ptr %"\D8\A7\D8\B3\D9\85") {
entry:
  %"\D8\A7\D8\B3\D9\852" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85", ptr %"\D8\A7\D8\B3\D9\852", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\852", ptr %alloca, align 8
  ret void
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %alloca = alloca i64, align 8
  %"%0.load" = load i64, ptr %alloca, align 8
  call void @"\D8\AD\D9\8A\D9\88\D8\A7\D9\86.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%0.load", ptr @str.const)
  %alloca1 = alloca i64, align 8
  %"%0.load2" = load i64, ptr %alloca, align 8
  store i64 %"%0.load2", ptr %alloca1, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
