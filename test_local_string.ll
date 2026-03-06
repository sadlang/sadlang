; ModuleID = 'test_local_string'
source_filename = "test_local_string"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@"\D9\86\D8\B51" = global i64 0
@"\D9\86\D8\B52" = global i64 0
@str.const = private unnamed_addr constant [11 x i8] c"\D9\85\D8\B1\D8\AD\D8\A8\D8\A7\00", align 1
@str.const.1 = private unnamed_addr constant [9 x i8] c"\D8\B9\D8\A7\D9\84\D9\85\00", align 1
@fmt.s = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@str.const.3 = private unnamed_addr constant [9 x i8] c"\D8\B1\D8\A8\D8\B7: \00", align 1
@str.const.4 = private unnamed_addr constant [2 x i8] c" \00", align 1

define void @__sad_main() {
entry:
  %"%\D9\86\D8\B52" = alloca ptr, align 8
  %"%\D9\86\D8\B51" = alloca ptr, align 8
  store ptr @str.const, ptr %"%\D9\86\D8\B51", align 8
  store ptr @str.const.1, ptr %"%\D9\86\D8\B52", align 8
  %"%\D9\86\D8\B51.load" = load ptr, ptr %"%\D9\86\D8\B51", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"%\D9\86\D8\B51.load")
  %"%\D9\86\D8\B52.load" = load ptr, ptr %"%\D9\86\D8\B52", align 8
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"%\D9\86\D8\B52.load")
  %"%\D9\86\D8\B51.load1" = load ptr, ptr %"%\D9\86\D8\B51", align 8
  %len1 = call i64 @strlen(ptr @str.const.3)
  %len2 = call i64 @strlen(ptr %"%\D9\86\D8\B51.load1")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = call ptr @malloc(i64 %bufsize)
  %2 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.3, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %3 = call ptr @memcpy(ptr %dest2, ptr %"%\D9\86\D8\B51.load1", i64 %copylen2)
  %len12 = call i64 @strlen(ptr %concat.buf)
  %len23 = call i64 @strlen(ptr @str.const.4)
  %totallen4 = add i64 %len12, %len23
  %bufsize5 = add i64 %totallen4, 1
  %concat.buf6 = call ptr @malloc(i64 %bufsize5)
  %4 = call ptr @memcpy(ptr %concat.buf6, ptr %concat.buf, i64 %len12)
  %dest27 = getelementptr i8, ptr %concat.buf6, i64 %len12
  %copylen28 = add i64 %len23, 1
  %5 = call ptr @memcpy(ptr %dest27, ptr @str.const.4, i64 %copylen28)
  %"%\D9\86\D8\B52.load9" = load ptr, ptr %"%\D9\86\D8\B52", align 8
  %len110 = call i64 @strlen(ptr %concat.buf6)
  %len211 = call i64 @strlen(ptr %"%\D9\86\D8\B52.load9")
  %totallen12 = add i64 %len110, %len211
  %bufsize13 = add i64 %totallen12, 1
  %concat.buf14 = call ptr @malloc(i64 %bufsize13)
  %6 = call ptr @memcpy(ptr %concat.buf14, ptr %concat.buf6, i64 %len110)
  %dest215 = getelementptr i8, ptr %concat.buf14, i64 %len110
  %copylen216 = add i64 %len211, 1
  %7 = call ptr @memcpy(ptr %dest215, ptr %"%\D9\86\D8\B52.load9", i64 %copylen216)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf14)
  ret void
}

declare i32 @printf(ptr, ...)

declare i64 @strlen(ptr)

declare ptr @malloc(i64)

declare ptr @memcpy(ptr, ptr, i64)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
