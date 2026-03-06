; ModuleID = 'test_exc_minimal'
source_filename = "test_exc_minimal"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@__sad_handler_stack = internal global [64 x ptr] zeroinitializer
@__sad_handler_count = internal global i32 0
@str.const = private unnamed_addr constant [4 x i8] c"\D8\A3=\00", align 1
@int.fmt = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@fmt.s = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@__sad_exception_msg = internal global ptr null
@str.const.1 = private unnamed_addr constant [7 x i8] c"\D8\AE\D8\B7\D8\A3\00", align 1

define i64 @"\D9\85\D8\B6\D8\A7\D8\B9\D9\81\D8\A9"(i64 %"\D9\86") {
entry:
  %"\D9\861" = alloca i64, align 8
  store i64 %"\D9\86", ptr %"\D9\861", align 8
  %"%\D9\86.load" = load i64, ptr %"\D9\861", align 8
  %shltmp = shl i64 %"%\D9\86.load", 1
  ret i64 %shltmp
}

define void @__sad_main() {
entry:
  %"%\D8\AE" = alloca ptr, align 8
  %"%\D8\A3" = alloca i64, align 8
  %jmpbuf = alloca [256 x i8], align 16
  %"%3.load" = load [256 x i8], ptr %jmpbuf, align 1
  %handler_count = load i32, ptr @__sad_handler_count, align 4
  %handler_slot = getelementptr [64 x ptr], ptr @__sad_handler_stack, i32 0, i32 %handler_count
  store ptr %jmpbuf, ptr %handler_slot, align 8
  %new_count = add i32 %handler_count, 1
  store i32 %new_count, ptr @__sad_handler_count, align 4
  %"%3.load1" = load [256 x i8], ptr %jmpbuf, align 1
  %setjmp_result = call i32 @_setjmp(ptr %jmpbuf, ptr null)
  %setjmp_result64 = sext i32 %setjmp_result to i64
  %cmpnetmp = icmp ne i64 %setjmp_result64, 0
  br i1 %cmpnetmp, label %catch_body_1, label %try_body_0

catch_body_1:                                     ; preds = %entry
  %handler_count4 = load i32, ptr @__sad_handler_count, align 4
  %new_count5 = sub i32 %handler_count4, 1
  store i32 %new_count5, ptr @__sad_handler_count, align 4
  %exception_msg = load ptr, ptr @__sad_exception_msg, align 8
  store ptr %exception_msg, ptr %"%\D8\AE", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %try_exit_3

try_body_0:                                       ; preds = %entry
  %calltmp = call i64 @"\D9\85\D8\B6\D8\A7\D8\B9\D9\81\D8\A9"(i64 5)
  store i64 %calltmp, ptr %"%\D8\A3", align 8
  %"%\D8\A3.load" = load i64, ptr %"%\D8\A3", align 8
  %strbuf = alloca i8, i64 32, align 1
  %1 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf, ptr @int.fmt, i64 %"%\D8\A3.load")
  %len1 = call i64 @strlen(ptr @str.const)
  %len2 = call i64 @strlen(ptr %strbuf)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = call ptr @malloc(i64 %bufsize)
  %2 = call ptr @memcpy(ptr %concat.buf, ptr @str.const, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %3 = call ptr @memcpy(ptr %dest2, ptr %strbuf, i64 %copylen2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %handler_count2 = load i32, ptr @__sad_handler_count, align 4
  %new_count3 = sub i32 %handler_count2, 1
  store i32 %new_count3, ptr @__sad_handler_count, align 4
  br label %try_exit_3

try_exit_3:                                       ; preds = %catch_body_1, %try_body_0
  ret void
}

; Function Attrs: returns_twice
declare i32 @_setjmp(ptr, ptr) #0

declare i32 @sprintf(ptr, ptr, ...)

declare i64 @strlen(ptr)

declare ptr @malloc(i64)

declare ptr @memcpy(ptr, ptr, i64)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}

attributes #0 = { returns_twice }
