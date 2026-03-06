; ModuleID = 'test_inherit'
source_filename = "test_inherit"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%"class.\D8\A3\D8\B3\D8\A7\D8\B3\D9\8A" = type { i64 }
%"class.\D9\85\D8\B4\D8\AA\D9\82" = type { i64, ptr }

@"\D9\83" = global i64 0
@str.const = private unnamed_addr constant [13 x i8] c"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\00", align 1
@str.const.1 = private unnamed_addr constant [14 x i8] c"\D8\A7\D9\84\D9\86\D9\88\D8\B9 = \00", align 1
@fmt.s = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@fmt.d = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@str.const.2 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.4 = private unnamed_addr constant [14 x i8] c"\D8\A7\D9\84\D8\A7\D8\B3\D9\85 = \00", align 1

define void @"\D8\A3\D8\B3\D8\A7\D8\B3\D9\8A.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, i64 %"\D8\A3") {
entry:
  %"\D8\A32" = alloca i64, align 8
  store i64 %"\D8\A3", ptr %"\D8\A32", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %"%\D8\A3.load" = load i64, ptr %"\D8\A32", align 8
  %"%self.ptrval" = load i64, ptr %self1, align 8
  %"%self.objptr" = inttoptr i64 %"%self.ptrval" to ptr
  %"\D8\A7\D9\84\D9\86\D9\88\D8\B9_gep" = getelementptr inbounds %"class.\D8\A3\D8\B3\D8\A7\D8\B3\D9\8A", ptr %"%self.objptr", i32 0, i32 0
  store i64 %"%\D8\A3.load", ptr %"\D8\A7\D9\84\D9\86\D9\88\D8\B9_gep", align 8
  ret void
}

define void @"\D9\85\D8\B4\D8\AA\D9\82.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, ptr %"\D9\86") {
entry:
  %"\D9\862" = alloca ptr, align 8
  store ptr %"\D9\86", ptr %"\D9\862", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %"%self.ptrval" = load i64, ptr %self1, align 8
  %"%self.objptr" = inttoptr i64 %"%self.ptrval" to ptr
  %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85_gep" = getelementptr inbounds %"class.\D9\85\D8\B4\D8\AA\D9\82", ptr %"%self.objptr", i32 0, i32 1
  store ptr %"\D9\862", ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85_gep", align 8
  ret void
}

define void @__sad_main() {
entry:
  %"%\D9\83" = alloca i64, align 8
  %"%0" = alloca %"class.\D9\85\D8\B4\D8\AA\D9\82", align 8
  %arg.ptrtoint = ptrtoint ptr %"%0" to i64
  call void @"\D9\85\D8\B4\D8\AA\D9\82.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %arg.ptrtoint, ptr @str.const)
  %obj.ptrtoint = ptrtoint ptr %"%0" to i64
  store i64 %obj.ptrtoint, ptr %"%\D9\83", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\83.ptrval" = load i64, ptr %"%\D9\83", align 8
  %"%\D9\83.objptr" = inttoptr i64 %"%\D9\83.ptrval" to ptr
  %"\D8\A7\D9\84\D9\86\D9\88\D8\B9_gep" = getelementptr inbounds %"class.\D9\85\D8\B4\D8\AA\D9\82", ptr %"%\D9\83.objptr", i32 0, i32 0
  %"\D8\A7\D9\84\D9\86\D9\88\D8\B9.val" = load i64, ptr %"\D8\A7\D9\84\D9\86\D9\88\D8\B9_gep", align 8
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"\D8\A7\D9\84\D9\86\D9\88\D8\B9.val")
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.4)
  %"%\D9\83.ptrval1" = load i64, ptr %"%\D9\83", align 8
  %"%\D9\83.objptr2" = inttoptr i64 %"%\D9\83.ptrval1" to ptr
  %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85_gep" = getelementptr inbounds %"class.\D9\85\D8\B4\D8\AA\D9\82", ptr %"%\D9\83.objptr2", i32 0, i32 1
  %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85.val" = load ptr, ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85_gep", align 8
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85.val")
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret void
}

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
