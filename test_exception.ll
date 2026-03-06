; ModuleID = 'test_exception'
source_filename = "test_exception"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [7 x i8] c"\D8\AE\D8\B7\D8\A3\00", align 1
@str.const.1 = private unnamed_addr constant [23 x i8] c"\D9\82\D8\B3\D9\85\D8\A9 \D8\B9\D9\84\D9\89 \D8\B5\D9\81\D8\B1\00", align 1
@__sad_exception_type = internal global ptr null
@__sad_exception_msg = internal global ptr null
@__sad_handler_stack = internal global [64 x ptr] zeroinitializer
@__sad_handler_count = internal global i32 0
@str.const.2 = private unnamed_addr constant [43 x i8] c"=== \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 1: try/catch \D8\A8\D8\B3\D9\8A\D8\B7 ===\00", align 1
@fmt.s = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@str.const.3 = private unnamed_addr constant [13 x i8] c"\D8\AF\D8\A7\D8\AE\D9\84 try\00", align 1
@str.const.6 = private unnamed_addr constant [20 x i8] c"\D8\AE\D8\B7\D8\A3 \D8\AA\D8\AC\D8\B1\D9\8A\D8\A8\D9\8A\00", align 1
@str.const.7 = private unnamed_addr constant [13 x i8] c"\D8\A7\D9\85\D8\B3\D9\83\D8\AA: \00", align 1
@str.const.9 = private unnamed_addr constant [17 x i8] c"\D8\A8\D8\B9\D8\AF try/catch\00", align 1
@str.const.11 = private unnamed_addr constant [52 x i8] c"=== \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 2: try \D8\A8\D8\AF\D9\88\D9\86 \D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1 ===\00", align 1
@str.const.13 = private unnamed_addr constant [13 x i8] c"try \D9\86\D8\A7\D8\AC\D8\AD\00", align 1
@str.const.15 = private unnamed_addr constant [29 x i8] c"\D9\84\D8\A7 \D9\8A\D9\88\D8\AC\D8\AF \D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1\00", align 1
@str.const.17 = private unnamed_addr constant [19 x i8] c"\D9\84\D9\86 \D9\8A\D8\B5\D9\84 \D9\87\D9\86\D8\A7\00", align 1
@str.const.19 = private unnamed_addr constant [20 x i8] c"\D8\A8\D8\B9\D8\AF try \D9\86\D8\A7\D8\AC\D8\AD\00", align 1
@str.const.21 = private unnamed_addr constant [53 x i8] c"=== \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 3: \D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1 \D9\85\D9\86 \D8\AF\D8\A7\D9\84\D8\A9 ===\00", align 1
@str.const.25 = private unnamed_addr constant [17 x i8] c"\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1: \00", align 1
@str.const.27 = private unnamed_addr constant [47 x i8] c"=== \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 4: try/catch \D9\85\D8\AA\D8\AF\D8\A7\D8\AE\D9\84 ===\00", align 1
@str.const.29 = private unnamed_addr constant [15 x i8] c"try \D8\AE\D8\A7\D8\B1\D8\AC\D9\8A\00", align 1
@str.const.31 = private unnamed_addr constant [15 x i8] c"try \D8\AF\D8\A7\D8\AE\D9\84\D9\8A\00", align 1
@str.const.34 = private unnamed_addr constant [18 x i8] c"\D8\AE\D8\B7\D8\A3 \D8\AF\D8\A7\D8\AE\D9\84\D9\8A\00", align 1
@str.const.35 = private unnamed_addr constant [22 x i8] c"\D8\A7\D9\85\D8\B3\D9\83 \D8\AF\D8\A7\D8\AE\D9\84\D9\8A: \00", align 1
@str.const.37 = private unnamed_addr constant [22 x i8] c"\D8\A8\D8\B9\D8\AF try \D8\AF\D8\A7\D8\AE\D9\84\D9\8A\00", align 1
@str.const.41 = private unnamed_addr constant [33 x i8] c"=== \D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9: \D9\86\D8\AC\D8\A7\D8\AD ===\00", align 1

define i64 @"\D9\82\D8\B3\D9\85\D8\A9"(i64 %"\D8\A3", i64 %"\D8\A8") {
entry:
  %"\D8\A82" = alloca i64, align 8
  store i64 %"\D8\A8", ptr %"\D8\A82", align 8
  %"\D8\A31" = alloca i64, align 8
  store i64 %"\D8\A3", ptr %"\D8\A31", align 8
  %"%\D8\A8.load" = load i64, ptr %"\D8\A82", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\A8.load", 0
  br i1 %cmpeqtmp, label %then_0, label %merge_1

merge_1:                                          ; preds = %entry
  %"%\D8\A3.load" = load i64, ptr %"\D8\A31", align 8
  %"%\D8\A8.load3" = load i64, ptr %"\D8\A82", align 8
  %divtmp = sdiv i64 %"%\D8\A3.load", %"%\D8\A8.load3"
  ret i64 %divtmp

then_0:                                           ; preds = %entry
  store ptr @str.const, ptr @__sad_exception_type, align 8
  store ptr @str.const.1, ptr @__sad_exception_msg, align 8
  %handler_count = load i32, ptr @__sad_handler_count, align 4
  %handler_idx = sub i32 %handler_count, 1
  %handler_slot = getelementptr [64 x ptr], ptr @__sad_handler_stack, i32 0, i32 %handler_idx
  %jmpbuf = load ptr, ptr %handler_slot, align 8
  call void @longjmp(ptr %jmpbuf, i32 1)
  unreachable

dead.after_raise:                                 ; No predecessors!
  unreachable
}

define void @__sad_main() {
entry:
  %"%\D8\AE2" = alloca ptr, align 8
  %"%\D8\AE1" = alloca ptr, align 8
  %jmpbuf48 = alloca [256 x i8], align 16
  %jmpbuf40 = alloca [256 x i8], align 16
  %"%\D8\A7\D9\84\D8\B1\D8\B3\D8\A7\D9\84\D8\A9" = alloca ptr, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %jmpbuf20 = alloca [256 x i8], align 16
  %"%\D8\B1" = alloca ptr, align 8
  %jmpbuf7 = alloca [256 x i8], align 16
  %"%\D8\A7\D9\84\D8\AE\D8\B7\D8\A3" = alloca ptr, align 8
  %jmpbuf = alloca [256 x i8], align 16
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%6.load" = load [256 x i8], ptr %jmpbuf, align 1
  %handler_count = load i32, ptr @__sad_handler_count, align 4
  %handler_slot = getelementptr [64 x ptr], ptr @__sad_handler_stack, i32 0, i32 %handler_count
  store ptr %jmpbuf, ptr %handler_slot, align 8
  %new_count = add i32 %handler_count, 1
  store i32 %new_count, ptr @__sad_handler_count, align 4
  %"%6.load1" = load [256 x i8], ptr %jmpbuf, align 1
  %setjmp_result = call i32 @_setjmp(ptr %jmpbuf, ptr null)
  %setjmp_result64 = sext i32 %setjmp_result to i64
  %cmpnetmp = icmp ne i64 %setjmp_result64, 0
  br i1 %cmpnetmp, label %catch_setup_4, label %try_body_3

catch_body_0_18:                                  ; preds = %catch_check_0_17
  %exception_msg19 = load ptr, ptr @__sad_exception_msg, align 8
  store ptr %exception_msg19, ptr %"%\D8\B1", align 8
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.17)
  br label %try_exit_15

catch_body_0_26:                                  ; preds = %catch_check_0_25
  %exception_msg32 = load ptr, ptr @__sad_exception_msg, align 8
  store ptr %exception_msg32, ptr %"%\D8\A7\D9\84\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", align 8
  %"%\D8\A7\D9\84\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.load" = load ptr, ptr %"%\D8\A7\D9\84\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", align 8
  %len133 = call i64 @strlen(ptr @str.const.25)
  %len234 = call i64 @strlen(ptr %"%\D8\A7\D9\84\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.load")
  %totallen35 = add i64 %len133, %len234
  %bufsize36 = add i64 %totallen35, 1
  %concat.buf37 = call ptr @malloc(i64 %bufsize36)
  %2 = call ptr @memcpy(ptr %concat.buf37, ptr @str.const.25, i64 %len133)
  %dest238 = getelementptr i8, ptr %concat.buf37, i64 %len133
  %copylen239 = add i64 %len234, 1
  %3 = call ptr @memcpy(ptr %dest238, ptr %"%\D8\A7\D9\84\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.load", i64 %copylen239)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf37)
  br label %try_exit_23

catch_body_0_34:                                  ; preds = %catch_check_0_33
  %exception_msg75 = load ptr, ptr @__sad_exception_msg, align 8
  store ptr %exception_msg75, ptr %"%\D8\AE2", align 8
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.17)
  br label %try_exit_31

catch_body_0_41:                                  ; preds = %catch_check_0_40
  %exception_msg63 = load ptr, ptr @__sad_exception_msg, align 8
  store ptr %exception_msg63, ptr %"%\D8\AE1", align 8
  %"%\D8\AE1.load" = load ptr, ptr %"%\D8\AE1", align 8
  %len164 = call i64 @strlen(ptr @str.const.35)
  %len265 = call i64 @strlen(ptr %"%\D8\AE1.load")
  %totallen66 = add i64 %len164, %len265
  %bufsize67 = add i64 %totallen66, 1
  %concat.buf68 = call ptr @malloc(i64 %bufsize67)
  %6 = call ptr @memcpy(ptr %concat.buf68, ptr @str.const.35, i64 %len164)
  %dest269 = getelementptr i8, ptr %concat.buf68, i64 %len164
  %copylen270 = add i64 %len265, 1
  %7 = call ptr @memcpy(ptr %dest269, ptr %"%\D8\AE1.load", i64 %copylen270)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf68)
  br label %try_exit_38

catch_body_0_9:                                   ; preds = %catch_check_0_8
  %exception_msg = load ptr, ptr @__sad_exception_msg, align 8
  store ptr %exception_msg, ptr %"%\D8\A7\D9\84\D8\AE\D8\B7\D8\A3", align 8
  %"%\D8\A7\D9\84\D8\AE\D8\B7\D8\A3.load" = load ptr, ptr %"%\D8\A7\D9\84\D8\AE\D8\B7\D8\A3", align 8
  %len1 = call i64 @strlen(ptr @str.const.7)
  %len2 = call i64 @strlen(ptr %"%\D8\A7\D9\84\D8\AE\D8\B7\D8\A3.load")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = call ptr @malloc(i64 %bufsize)
  %9 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.7, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %10 = call ptr @memcpy(ptr %dest2, ptr %"%\D8\A7\D9\84\D8\AE\D8\B7\D8\A3.load", i64 %copylen2)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  br label %try_exit_6

catch_check_0_17:                                 ; preds = %catch_setup_13
  br label %catch_body_0_18

catch_check_0_25:                                 ; preds = %catch_setup_21
  br label %catch_body_0_26

catch_check_0_33:                                 ; preds = %catch_setup_29
  br label %catch_body_0_34

catch_check_0_40:                                 ; preds = %catch_setup_36
  br label %catch_body_0_41

catch_check_0_8:                                  ; preds = %catch_setup_4
  br label %catch_body_0_9

catch_setup_13:                                   ; preds = %try_exit_6
  %handler_count17 = load i32, ptr @__sad_handler_count, align 4
  %new_count18 = sub i32 %handler_count17, 1
  store i32 %new_count18, ptr @__sad_handler_count, align 4
  br label %catch_check_0_17

catch_setup_21:                                   ; preds = %try_exit_15
  %handler_count30 = load i32, ptr @__sad_handler_count, align 4
  %new_count31 = sub i32 %handler_count30, 1
  store i32 %new_count31, ptr @__sad_handler_count, align 4
  br label %catch_check_0_25

catch_setup_29:                                   ; preds = %try_exit_23
  %handler_count73 = load i32, ptr @__sad_handler_count, align 4
  %new_count74 = sub i32 %handler_count73, 1
  store i32 %new_count74, ptr @__sad_handler_count, align 4
  br label %catch_check_0_33

catch_setup_36:                                   ; preds = %try_body_28
  %handler_count61 = load i32, ptr @__sad_handler_count, align 4
  %new_count62 = sub i32 %handler_count61, 1
  store i32 %new_count62, ptr @__sad_handler_count, align 4
  br label %catch_check_0_40

catch_setup_4:                                    ; preds = %entry
  %handler_count5 = load i32, ptr @__sad_handler_count, align 4
  %new_count6 = sub i32 %handler_count5, 1
  store i32 %new_count6, ptr @__sad_handler_count, align 4
  br label %catch_check_0_8

try_body_12:                                      ; preds = %try_exit_6
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.13)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.15)
  %handler_count15 = load i32, ptr @__sad_handler_count, align 4
  %new_count16 = sub i32 %handler_count15, 1
  store i32 %new_count16, ptr @__sad_handler_count, align 4
  br label %try_exit_15

try_body_20:                                      ; preds = %try_exit_15
  %calltmp = call i64 @"\D9\82\D8\B3\D9\85\D8\A9"(i64 10, i64 0)
  store i64 %calltmp, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.17)
  %handler_count28 = load i32, ptr @__sad_handler_count, align 4
  %new_count29 = sub i32 %handler_count28, 1
  store i32 %new_count29, ptr @__sad_handler_count, align 4
  br label %try_exit_23

try_body_28:                                      ; preds = %try_exit_23
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.29)
  %"%41.load" = load [256 x i8], ptr %jmpbuf48, align 1
  %handler_count49 = load i32, ptr @__sad_handler_count, align 4
  %handler_slot50 = getelementptr [64 x ptr], ptr @__sad_handler_stack, i32 0, i32 %handler_count49
  store ptr %jmpbuf48, ptr %handler_slot50, align 8
  %new_count51 = add i32 %handler_count49, 1
  store i32 %new_count51, ptr @__sad_handler_count, align 4
  %"%41.load52" = load [256 x i8], ptr %jmpbuf48, align 1
  %setjmp_result53 = call i32 @_setjmp(ptr %jmpbuf48, ptr null)
  %setjmp_result6454 = sext i32 %setjmp_result53 to i64
  %cmpnetmp55 = icmp ne i64 %setjmp_result6454, 0
  br i1 %cmpnetmp55, label %catch_setup_36, label %try_body_35

try_body_3:                                       ; preds = %entry
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  store ptr @str.const, ptr @__sad_exception_type, align 8
  store ptr @str.const.6, ptr @__sad_exception_msg, align 8
  %handler_count2 = load i32, ptr @__sad_handler_count, align 4
  %handler_idx = sub i32 %handler_count2, 1
  %handler_slot3 = getelementptr [64 x ptr], ptr @__sad_handler_stack, i32 0, i32 %handler_idx
  %jmpbuf4 = load ptr, ptr %handler_slot3, align 8
  call void @longjmp(ptr %jmpbuf4, i32 1)
  unreachable

try_body_35:                                      ; preds = %try_body_28
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.31)
  store ptr @str.const, ptr @__sad_exception_type, align 8
  store ptr @str.const.34, ptr @__sad_exception_msg, align 8
  %handler_count56 = load i32, ptr @__sad_handler_count, align 4
  %handler_idx57 = sub i32 %handler_count56, 1
  %handler_slot58 = getelementptr [64 x ptr], ptr @__sad_handler_stack, i32 0, i32 %handler_idx57
  %jmpbuf59 = load ptr, ptr %handler_slot58, align 8
  call void @longjmp(ptr %jmpbuf59, i32 1)
  unreachable

try_exit_15:                                      ; preds = %catch_body_0_18, %try_body_12
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.19)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.21)
  %"%26.load" = load [256 x i8], ptr %jmpbuf20, align 1
  %handler_count21 = load i32, ptr @__sad_handler_count, align 4
  %handler_slot22 = getelementptr [64 x ptr], ptr @__sad_handler_stack, i32 0, i32 %handler_count21
  store ptr %jmpbuf20, ptr %handler_slot22, align 8
  %new_count23 = add i32 %handler_count21, 1
  store i32 %new_count23, ptr @__sad_handler_count, align 4
  %"%26.load24" = load [256 x i8], ptr %jmpbuf20, align 1
  %setjmp_result25 = call i32 @_setjmp(ptr %jmpbuf20, ptr null)
  %setjmp_result6426 = sext i32 %setjmp_result25 to i64
  %cmpnetmp27 = icmp ne i64 %setjmp_result6426, 0
  br i1 %cmpnetmp27, label %catch_setup_21, label %try_body_20

try_exit_23:                                      ; preds = %catch_body_0_26, %try_body_20
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.27)
  %"%37.load" = load [256 x i8], ptr %jmpbuf40, align 1
  %handler_count41 = load i32, ptr @__sad_handler_count, align 4
  %handler_slot42 = getelementptr [64 x ptr], ptr @__sad_handler_stack, i32 0, i32 %handler_count41
  store ptr %jmpbuf40, ptr %handler_slot42, align 8
  %new_count43 = add i32 %handler_count41, 1
  store i32 %new_count43, ptr @__sad_handler_count, align 4
  %"%37.load44" = load [256 x i8], ptr %jmpbuf40, align 1
  %setjmp_result45 = call i32 @_setjmp(ptr %jmpbuf40, ptr null)
  %setjmp_result6446 = sext i32 %setjmp_result45 to i64
  %cmpnetmp47 = icmp ne i64 %setjmp_result6446, 0
  br i1 %cmpnetmp47, label %catch_setup_29, label %try_body_28

try_exit_31:                                      ; preds = %catch_body_0_34, %try_exit_38
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.41)
  ret void

try_exit_38:                                      ; preds = %catch_body_0_41
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.37)
  %handler_count71 = load i32, ptr @__sad_handler_count, align 4
  %new_count72 = sub i32 %handler_count71, 1
  store i32 %new_count72, ptr @__sad_handler_count, align 4
  br label %try_exit_31

try_exit_6:                                       ; preds = %catch_body_0_9
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.9)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.11)
  %"%17.load" = load [256 x i8], ptr %jmpbuf7, align 1
  %handler_count8 = load i32, ptr @__sad_handler_count, align 4
  %handler_slot9 = getelementptr [64 x ptr], ptr @__sad_handler_stack, i32 0, i32 %handler_count8
  store ptr %jmpbuf7, ptr %handler_slot9, align 8
  %new_count10 = add i32 %handler_count8, 1
  store i32 %new_count10, ptr @__sad_handler_count, align 4
  %"%17.load11" = load [256 x i8], ptr %jmpbuf7, align 1
  %setjmp_result12 = call i32 @_setjmp(ptr %jmpbuf7, ptr null)
  %setjmp_result6413 = sext i32 %setjmp_result12 to i64
  %cmpnetmp14 = icmp ne i64 %setjmp_result6413, 0
  br i1 %cmpnetmp14, label %catch_setup_13, label %try_body_12

dead.after_raise:                                 ; No predecessors!
  unreachable

dead.after_raise60:                               ; No predecessors!
  unreachable
}

; Function Attrs: noreturn
declare void @longjmp(ptr, i32) #0

declare i32 @printf(ptr, ...)

; Function Attrs: returns_twice
declare i32 @_setjmp(ptr, ptr) #1

declare i64 @strlen(ptr)

declare ptr @malloc(i64)

declare ptr @memcpy(ptr, ptr, i64)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}

attributes #0 = { noreturn }
attributes #1 = { returns_twice }
