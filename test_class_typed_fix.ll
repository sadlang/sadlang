; ModuleID = 'test_class_typed'
source_filename = "test_class_typed"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%"class.\D8\B4\D8\AE\D8\B5" = type { ptr, i64 }

@"\D8\B4" = global i64 0
@str.const = private unnamed_addr constant [13 x i8] c"\D8\A7\D9\84\D8\A7\D8\B3\D9\85: \00", align 1
@fmt.s = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@str.const.1 = private unnamed_addr constant [13 x i8] c"\D8\A7\D9\84\D8\B9\D9\85\D8\B1: \00", align 1
@int.fmt = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@str.const.3 = private unnamed_addr constant [9 x i8] c"\D8\B3\D8\A7\D8\B1\D8\A9\00", align 1

define void @"\D8\B4\D8\AE\D8\B5.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, i64 %"\D8\A7\D8\B3\D9\85", i64 %"\D8\B9\D9\85\D8\B1") {
entry:
  %"\D8\B9\D9\85\D8\B13" = alloca i64, align 8
  store i64 %"\D8\B9\D9\85\D8\B1", ptr %"\D8\B9\D9\85\D8\B13", align 8
  %"\D8\A7\D8\B3\D9\852" = alloca i64, align 8
  store i64 %"\D8\A7\D8\B3\D9\85", ptr %"\D8\A7\D8\B3\D9\852", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %"%\D8\A7\D8\B3\D9\85.load" = load i64, ptr %"\D8\A7\D8\B3\D9\852", align 8
  %"%self.ptrval" = load i64, ptr %self1, align 8
  %"%self.objptr" = inttoptr i64 %"%self.ptrval" to ptr
  %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85_gep" = getelementptr inbounds %"class.\D8\B4\D8\AE\D8\B5", ptr %"%self.objptr", i32 0, i32 0
  %int2ptr = inttoptr i64 %"%\D8\A7\D8\B3\D9\85.load" to ptr
  store ptr %int2ptr, ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85_gep", align 8
  %"%\D8\B9\D9\85\D8\B1.load" = load i64, ptr %"\D8\B9\D9\85\D8\B13", align 8
  %"%self.ptrval4" = load i64, ptr %self1, align 8
  %"%self.objptr5" = inttoptr i64 %"%self.ptrval4" to ptr
  %"\D8\A7\D9\84\D8\B9\D9\85\D8\B1_gep" = getelementptr inbounds %"class.\D8\B4\D8\AE\D8\B5", ptr %"%self.objptr5", i32 0, i32 1
  store i64 %"%\D8\B9\D9\85\D8\B1.load", ptr %"\D8\A7\D9\84\D8\B9\D9\85\D8\B1_gep", align 8
  ret void
}

define void @"\D8\B4\D8\AE\D8\B5.\D8\B9\D8\B1\D8\B6"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %"%self.ptrval" = load i64, ptr %self1, align 8
  %"%self.objptr" = inttoptr i64 %"%self.ptrval" to ptr
  %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85_gep" = getelementptr inbounds %"class.\D8\B4\D8\AE\D8\B5", ptr %"%self.objptr", i32 0, i32 0
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
  %"%self.ptrval2" = load i64, ptr %self1, align 8
  %"%self.objptr3" = inttoptr i64 %"%self.ptrval2" to ptr
  %"\D8\A7\D9\84\D8\B9\D9\85\D8\B1_gep" = getelementptr inbounds %"class.\D8\B4\D8\AE\D8\B5", ptr %"%self.objptr3", i32 0, i32 1
  %"\D8\A7\D9\84\D8\B9\D9\85\D8\B1.val" = load i64, ptr %"\D8\A7\D9\84\D8\B9\D9\85\D8\B1_gep", align 8
  %strbuf = alloca i8, i64 32, align 1
  %3 = call i32 (ptr, i64, ptr, ...) @sprintf(ptr %strbuf, ptr @int.fmt, i64 %"\D8\A7\D9\84\D8\B9\D9\85\D8\B1.val")
  %len14 = call i64 @strlen(ptr @str.const.1)
  %len25 = call i64 @strlen(ptr %strbuf)
  %totallen6 = add i64 %len14, %len25
  %bufsize7 = add i64 %totallen6, 1
  %concat.buf8 = call ptr @malloc(i64 %bufsize7)
  %4 = call ptr @memcpy(ptr %concat.buf8, ptr @str.const.1, i64 %len14)
  %dest29 = getelementptr i8, ptr %concat.buf8, i64 %len14
  %copylen210 = add i64 %len25, 1
  %5 = call ptr @memcpy(ptr %dest29, ptr %strbuf, i64 %copylen210)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf8)
  ret void
}

define void @__sad_main() {
entry:
  %"%\D8\B4" = alloca i64, align 8
  %"%6" = alloca %"class.\D8\B4\D8\AE\D8\B5", align 8
  %arg.ptrtoint = ptrtoint ptr %"%6" to i64
  call void @"\D8\B4\D8\AE\D8\B5.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %arg.ptrtoint, i64 ptrtoint (ptr @str.const.3 to i64), i64 25)
  %obj.ptrtoint = ptrtoint ptr %"%6" to i64
  store i64 %obj.ptrtoint, ptr %"%\D8\B4", align 8
  %"%\D8\B4.load" = load i64, ptr %"%\D8\B4", align 8
  call void @"\D8\B4\D8\AE\D8\B5.\D8\B9\D8\B1\D8\B6"(i64 %"%\D8\B4.load")
  ret void
}

declare i64 @strlen(ptr)

declare ptr @malloc(i64)

declare ptr @memcpy(ptr, ptr, i64)

declare i32 @printf(ptr, ...)

declare i32 @sprintf(ptr, ptr, ...)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
