; ModuleID = 'test_exc_simple'
source_filename = "test_exc_simple"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@__sad_handler_stack = internal global [64 x ptr] zeroinitializer
@__sad_handler_count = internal global i32 0
@str.const = private unnamed_addr constant [7 x i8] c"\D8\AE\D8\B7\D8\A3\00", align 1
@str.const.1 = private unnamed_addr constant [8 x i8] c"\D8\AE\D8\B7\D8\A31\00", align 1
@__sad_exception_type = internal global ptr null
@__sad_exception_msg = internal global ptr null
@fmt.d = private unnamed_addr constant [5 x i8] c"%lld\00", align 1

define void @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B11"() {
entry:
  %"%\D8\AE" = alloca ptr, align 8
  %jmpbuf = alloca [256 x i8], align 16
  %"%0.load" = load [256 x i8], ptr %jmpbuf, align 1
  %handler_count = load i32, ptr @__sad_handler_count, align 4
  %handler_slot = getelementptr [64 x ptr], ptr @__sad_handler_stack, i32 0, i32 %handler_count
  store ptr %jmpbuf, ptr %handler_slot, align 8
  %new_count = add i32 %handler_count, 1
  store i32 %new_count, ptr @__sad_handler_count, align 4
  %"%0.load1" = load [256 x i8], ptr %jmpbuf, align 1
  %setjmp_result = call i32 @_setjmp(ptr %jmpbuf, ptr null)
  %setjmp_result64 = sext i32 %setjmp_result to i64
  %cmpnetmp = icmp ne i64 %setjmp_result64, 0
  br i1 %cmpnetmp, label %catch_setup_1, label %try_body_0

catch_body_0_6:                                   ; preds = %catch_check_0_5
  %exception_msg = load ptr, ptr @__sad_exception_msg, align 8
  store ptr %exception_msg, ptr %"%\D8\AE", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 1)
  br label %try_exit_3

catch_check_0_5:                                  ; preds = %catch_setup_1
  br label %catch_body_0_6

catch_setup_1:                                    ; preds = %entry
  %handler_count5 = load i32, ptr @__sad_handler_count, align 4
  %new_count6 = sub i32 %handler_count5, 1
  store i32 %new_count6, ptr @__sad_handler_count, align 4
  br label %catch_check_0_5

try_body_0:                                       ; preds = %entry
  store ptr @str.const, ptr @__sad_exception_type, align 8
  store ptr @str.const.1, ptr @__sad_exception_msg, align 8
  %handler_count2 = load i32, ptr @__sad_handler_count, align 4
  %handler_idx = sub i32 %handler_count2, 1
  %handler_slot3 = getelementptr [64 x ptr], ptr @__sad_handler_stack, i32 0, i32 %handler_idx
  %jmpbuf4 = load ptr, ptr %handler_slot3, align 8
  call void @longjmp(ptr %jmpbuf4, i32 1)
  unreachable

try_exit_3:                                       ; preds = %catch_body_0_6
  ret void

dead.after_raise:                                 ; No predecessors!
  unreachable
}

define void @__sad_main() {
entry:
  call void @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B11"()
  ret void
}

; Function Attrs: returns_twice
declare i32 @_setjmp(ptr, ptr) #0

; Function Attrs: noreturn
declare void @longjmp(ptr, i32) #1

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}

attributes #0 = { returns_twice }
attributes #1 = { noreturn }
