; ModuleID = 'test_exceptions_compile'
source_filename = "test_exceptions_compile"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [44 x i8] c"=== \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 \D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1\D8\A7\D8\AA ===\00", align 1
@fmt.s = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@str.const.1 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@__sad_handler_stack = internal global [64 x ptr] zeroinitializer
@__sad_handler_count = internal global i32 0
@str.const.3 = private unnamed_addr constant [18 x i8] c"\D9\82\D8\A8\D9\84 \D8\A7\D9\84\D8\AE\D8\B7\D8\A3\00", align 1
@str.const.7 = private unnamed_addr constant [7 x i8] c"\D8\AE\D8\B7\D8\A3\00", align 1
@str.const.8 = private unnamed_addr constant [21 x i8] c"\D8\AE\D8\B7\D8\A3 \D8\AA\D8\AC\D8\B1\D9\8A\D8\A8\D9\8A!\00", align 1
@__sad_exception_type = internal global ptr null
@__sad_exception_msg = internal global ptr null
@str.const.9 = private unnamed_addr constant [15 x i8] c"\D8\A7\D9\85\D8\B3\D9\83\D9\86\D8\A7: \00", align 1
@int.fmt = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@str.const.13 = private unnamed_addr constant [31 x i8] c"\D9\81\D9\8A \D8\A7\D9\84\D8\AD\D8\A7\D9\88\D9\84 \D8\A7\D9\84\D8\AB\D8\A7\D9\86\D9\8A\00", align 1
@str.const.17 = private unnamed_addr constant [19 x i8] c"\D9\84\D9\86 \D9\8A\D8\B5\D9\84 \D9\87\D9\86\D8\A7\00", align 1
@str.const.21 = private unnamed_addr constant [32 x i8] c"\D8\A3\D8\AE\D9\8A\D8\B1\D8\A7\D9\8B \D8\AA\D9\85 \D8\AA\D9\86\D9\81\D9\8A\D8\B0\D9\87!\00", align 1
@str.const.29 = private unnamed_addr constant [36 x i8] c"=== \D8\A7\D9\86\D8\AA\D9\87\D9\89 \D8\A7\D9\84\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 ===\00", align 1

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %"%\D8\AE27" = alloca ptr, align 8
  %jmpbuf19 = alloca [256 x i8], align 16
  %jmpbuf7 = alloca [256 x i8], align 16
  %"%\D8\AE" = alloca i64, align 8
  %jmpbuf = alloca [256 x i8], align 16
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%1.load" = load [256 x i8], ptr %jmpbuf, align 1
  %handler_count = load i32, ptr @__sad_handler_count, align 4
  %handler_slot = getelementptr [64 x ptr], ptr @__sad_handler_stack, i32 0, i32 %handler_count
  store ptr %jmpbuf, ptr %handler_slot, align 8
  %new_count = add i32 %handler_count, 1
  store i32 %new_count, ptr @__sad_handler_count, align 4
  %"%1.load1" = load [256 x i8], ptr %jmpbuf, align 1
  %setjmp_result = call i32 @_setjmp(ptr %jmpbuf, ptr null)
  %setjmp_result64 = sext i32 %setjmp_result to i64
  %cmpnetmp = icmp ne i64 %setjmp_result64, 0
  br i1 %cmpnetmp, label %catch_setup_1, label %try_body_0

catch_body_0_17:                                  ; preds = %catch_check_0_16
  %exception_msg28 = load ptr, ptr @__sad_exception_msg, align 8
  store ptr %exception_msg28, ptr %"%\D8\AE27", align 8
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.17)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %handler_count29 = load i32, ptr @__sad_handler_count, align 4
  %new_count30 = sub i32 %handler_count29, 1
  store i32 %new_count30, ptr @__sad_handler_count, align 4
  br label %finally_body_11

catch_body_0_6:                                   ; preds = %catch_check_0_5
  %exception_msg = load ptr, ptr @__sad_exception_msg, align 8
  %obj.ptrtoint = ptrtoint ptr %exception_msg to i64
  store i64 %obj.ptrtoint, ptr %"%\D8\AE", align 8
  %"%\D8\AE.load" = load i64, ptr %"%\D8\AE", align 8
  %strbuf = alloca i8, i64 32, align 1
  %4 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf, ptr @int.fmt, i64 %"%\D8\AE.load")
  %len1 = call i64 @strlen(ptr @str.const.9)
  %len2 = call i64 @strlen(ptr %strbuf)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = call ptr @malloc(i64 %bufsize)
  %5 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.9, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %6 = call ptr @memcpy(ptr %dest2, ptr %strbuf, i64 %copylen2)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %try_exit_3

catch_check_0_16:                                 ; preds = %catch_setup_10
  br label %catch_body_0_17

catch_check_0_5:                                  ; preds = %catch_setup_1
  br label %catch_body_0_6

catch_setup_1:                                    ; preds = %entry
  %handler_count5 = load i32, ptr @__sad_handler_count, align 4
  %new_count6 = sub i32 %handler_count5, 1
  store i32 %new_count6, ptr @__sad_handler_count, align 4
  br label %catch_check_0_5

catch_setup_10:                                   ; preds = %try_exit_3
  %handler_count17 = load i32, ptr @__sad_handler_count, align 4
  %new_count18 = sub i32 %handler_count17, 1
  store i32 %new_count18, ptr @__sad_handler_count, align 4
  %"%14.load" = load [256 x i8], ptr %jmpbuf19, align 1
  %handler_count20 = load i32, ptr @__sad_handler_count, align 4
  %handler_slot21 = getelementptr [64 x ptr], ptr @__sad_handler_stack, i32 0, i32 %handler_count20
  store ptr %jmpbuf19, ptr %handler_slot21, align 8
  %new_count22 = add i32 %handler_count20, 1
  store i32 %new_count22, ptr @__sad_handler_count, align 4
  %"%14.load23" = load [256 x i8], ptr %jmpbuf19, align 1
  %setjmp_result24 = call i32 @_setjmp(ptr %jmpbuf19, ptr null)
  %setjmp_result6425 = sext i32 %setjmp_result24 to i64
  %cmpnetmp26 = icmp ne i64 %setjmp_result6425, 0
  br i1 %cmpnetmp26, label %finally_rethrow_13, label %catch_check_0_16

finally_body_11:                                  ; preds = %catch_body_0_17, %try_body_9
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.21)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %try_exit_12

finally_rethrow_13:                               ; preds = %catch_setup_10
  %handler_count31 = load i32, ptr @__sad_handler_count, align 4
  %new_count32 = sub i32 %handler_count31, 1
  store i32 %new_count32, ptr @__sad_handler_count, align 4
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.21)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %handler_count33 = load i32, ptr @__sad_handler_count, align 4
  %handler_idx34 = sub i32 %handler_count33, 1
  %handler_slot35 = getelementptr [64 x ptr], ptr @__sad_handler_stack, i32 0, i32 %handler_idx34
  %jmpbuf36 = load ptr, ptr %handler_slot35, align 8
  call void @longjmp(ptr %jmpbuf36, i32 1)
  unreachable

try_body_0:                                       ; preds = %entry
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  store ptr @str.const.7, ptr @__sad_exception_type, align 8
  store ptr @str.const.8, ptr @__sad_exception_msg, align 8
  %handler_count2 = load i32, ptr @__sad_handler_count, align 4
  %handler_idx = sub i32 %handler_count2, 1
  %handler_slot3 = getelementptr [64 x ptr], ptr @__sad_handler_stack, i32 0, i32 %handler_idx
  %jmpbuf4 = load ptr, ptr %handler_slot3, align 8
  call void @longjmp(ptr %jmpbuf4, i32 1)
  unreachable

try_body_9:                                       ; preds = %try_exit_3
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.13)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %handler_count15 = load i32, ptr @__sad_handler_count, align 4
  %new_count16 = sub i32 %handler_count15, 1
  store i32 %new_count16, ptr @__sad_handler_count, align 4
  br label %finally_body_11

try_exit_12:                                      ; preds = %finally_body_11
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.29)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0

try_exit_3:                                       ; preds = %catch_body_0_6
  %"%10.load" = load [256 x i8], ptr %jmpbuf7, align 1
  %handler_count8 = load i32, ptr @__sad_handler_count, align 4
  %handler_slot9 = getelementptr [64 x ptr], ptr @__sad_handler_stack, i32 0, i32 %handler_count8
  store ptr %jmpbuf7, ptr %handler_slot9, align 8
  %new_count10 = add i32 %handler_count8, 1
  store i32 %new_count10, ptr @__sad_handler_count, align 4
  %"%10.load11" = load [256 x i8], ptr %jmpbuf7, align 1
  %setjmp_result12 = call i32 @_setjmp(ptr %jmpbuf7, ptr null)
  %setjmp_result6413 = sext i32 %setjmp_result12 to i64
  %cmpnetmp14 = icmp ne i64 %setjmp_result6413, 0
  br i1 %cmpnetmp14, label %catch_setup_10, label %try_body_9

dead.after_raise:                                 ; No predecessors!
  unreachable

dead.after_rethrow:                               ; No predecessors!
  unreachable
}

declare i32 @printf(ptr, ...)

; Function Attrs: returns_twice
declare i32 @_setjmp(ptr, ptr) #0

; Function Attrs: noreturn
declare void @longjmp(ptr, i32) #1

declare i32 @sprintf(ptr, ptr, ...)

declare i64 @strlen(ptr)

declare ptr @malloc(i64)

declare ptr @memcpy(ptr, ptr, i64)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}

attributes #0 = { returns_twice }
attributes #1 = { noreturn }
