; ModuleID = 'test_class_fix'
source_filename = "test_class_fix"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%"class.\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1" = type { ptr }

@"\D8\B4" = global i64 0
@str.const = private unnamed_addr constant [13 x i8] c"\D8\A7\D9\84\D8\A7\D8\B3\D9\85: \00", align 1
@fmt.s = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@str.const.1 = private unnamed_addr constant [9 x i8] c"\D8\A3\D8\AD\D9\85\D8\AF\00", align 1

define void @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, i64 %"\D8\A7\D8\B3\D9\85") {
entry:
  %"\D8\A7\D8\B3\D9\852" = alloca i64, align 8
  store i64 %"\D8\A7\D8\B3\D9\85", ptr %"\D8\A7\D8\B3\D9\852", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %"%\D8\A7\D8\B3\D9\85.load" = load i64, ptr %"\D8\A7\D8\B3\D9\852", align 8
  %"%self.ptrval" = load i64, ptr %self1, align 8
  %"%self.objptr" = inttoptr i64 %"%self.ptrval" to ptr
  %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85_gep" = getelementptr inbounds %"class.\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1", ptr %"%self.objptr", i32 0, i32 0
  %int2ptr = inttoptr i64 %"%\D8\A7\D8\B3\D9\85.load" to ptr
  store ptr %int2ptr, ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85_gep", align 8
  ret void
}

define void @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1.\D8\A7\D8\B9\D8\B1\D8\B6"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %"%self.ptrval" = load i64, ptr %self1, align 8
  %"%self.objptr" = inttoptr i64 %"%self.ptrval" to ptr
  %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85_gep" = getelementptr inbounds %"class.\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1", ptr %"%self.objptr", i32 0, i32 0
  %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85.val" = load ptr, ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85_gep", align 8
  %len1 = call i64 @strlen(ptr @str.const)
  %len2 = call i64 @strlen(ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85.val")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = call ptr @malloc(i64 %bufsize)
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85.val", i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  ret void
}

define void @__sad_main() {
entry:
  %"%\D8\B4" = alloca i64, align 8
  %"%3" = alloca %"class.\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1", align 8
  %arg.ptrtoint = ptrtoint ptr %"%3" to i64
  call void @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %arg.ptrtoint, i64 ptrtoint (ptr @str.const.1 to i64))
  %obj.ptrtoint = ptrtoint ptr %"%3" to i64
  store i64 %obj.ptrtoint, ptr %"%\D8\B4", align 8
  %"%\D8\B4.load" = load i64, ptr %"%\D8\B4", align 8
  call void @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1.\D8\A7\D8\B9\D8\B1\D8\B6"(i64 %"%\D8\B4.load")
  ret void
}

declare i64 @strlen(ptr)

declare ptr @malloc(i64)

declare ptr @memcpy(ptr, ptr, i64)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
