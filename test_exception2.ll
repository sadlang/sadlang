; ModuleID = 'test_exception2'
source_filename = "test_exception2"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [16 x i8] c"\D8\B1\D9\82\D9\85 \D8\B3\D8\A7\D9\84\D8\A8\00", align 1
@__sad_exception_msg = internal global ptr null
@__sad_handler_stack = internal global [64 x ptr] zeroinitializer
@__sad_handler_count = internal global i32 0
@str.const.1 = private unnamed_addr constant [29 x i8] c"\D9\82\D9\8A\D9\85\D8\A9 \D9\83\D8\A8\D9\8A\D8\B1\D8\A9 \D8\AC\D8\AF\D8\A7\D9\8B\00", align 1
@str.const.2 = private unnamed_addr constant [62 x i8] c"=== \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 5: \D8\AF\D8\A7\D9\84\D8\A9 \D8\AA\D8\B1\D9\85\D9\8A \D9\88\D8\A3\D8\AE\D8\B1\D9\89 \D8\AA\D9\85\D8\B3\D9\83 ===\00", align 1
@fmt.s = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@str.const.3 = private unnamed_addr constant [13 x i8] c"\D8\AA\D8\AD\D9\82\D9\82(5)=\00", align 1
@int.fmt = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@str.const.5 = private unnamed_addr constant [12 x i8] c"\D9\84\D9\86 \D9\8A\D8\B5\D9\84\00", align 1
@str.const.7 = private unnamed_addr constant [9 x i8] c"\D8\AE\D8\B7\D8\A3: \00", align 1
@str.const.9 = private unnamed_addr constant [60 x i8] c"=== \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 6: \D8\B9\D8\AF\D8\A9 \D8\AF\D9\88\D8\A7\D9\84 \D9\81\D9\8A \D8\A7\D9\84\D8\B3\D9\84\D8\B3\D9\84\D8\A9 ===\00", align 1
@str.const.11 = private unnamed_addr constant [5 x i8] c"\D9\861=\00", align 1
@str.const.16 = private unnamed_addr constant [11 x i8] c"\D8\A7\D9\85\D8\B3\D9\83: \00", align 1
@str.const.19 = private unnamed_addr constant [50 x i8] c"=== \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 7: try/catch \D8\A8\D8\AF\D9\88\D9\86 \D8\B1\D9\85\D9\8A ===\00", align 1
@str.const.21 = private unnamed_addr constant [14 x i8] c"\D8\A7\D9\84\D8\AD\D8\B3\D8\A7\D8\A8=\00", align 1
@str.const.26 = private unnamed_addr constant [23 x i8] c"=== \D8\A7\D9\84\D9\86\D9\87\D8\A7\D9\8A\D8\A9 ===\00", align 1

define i64 @"\D8\AA\D8\AD\D9\82\D9\82"(i64 %"\D9\86") {
entry:
  %"\D9\861" = alloca i64, align 8
  store i64 %"\D9\86", ptr %"\D9\861", align 8
  %"%\D9\86.load" = load i64, ptr %"\D9\861", align 8
  %cmplttmp = icmp slt i64 %"%\D9\86.load", 0
  br i1 %cmplttmp, label %then_0, label %merge_1

merge_1:                                          ; preds = %entry
  %"%\D9\86.load2" = load i64, ptr %"\D9\861", align 8
  %shltmp = shl i64 %"%\D9\86.load2", 1
  ret i64 %shltmp

then_0:                                           ; preds = %entry
  store ptr @str.const, ptr @__sad_exception_msg, align 8
  %handler_count = load i32, ptr @__sad_handler_count, align 4
  %handler_idx = sub i32 %handler_count, 1
  %handler_slot = getelementptr [64 x ptr], ptr @__sad_handler_stack, i32 0, i32 %handler_idx
  %jmpbuf = load ptr, ptr %handler_slot, align 8
  call void @longjmp(ptr %jmpbuf, i32 1)
  unreachable

dead.after_raise:                                 ; No predecessors!
  unreachable
}

define i64 @"\D8\AF\D8\A7\D9\84\D8\A9_\D8\A3"(i64 %"\D8\B3") {
entry:
  %"\D8\B31" = alloca i64, align 8
  store i64 %"\D8\B3", ptr %"\D8\B31", align 8
  %"%\D8\B3.load" = load i64, ptr %"\D8\B31", align 8
  %addtmp = add i64 %"%\D8\B3.load", 10
  ret i64 %addtmp
}

define i64 @"\D8\AF\D8\A7\D9\84\D8\A9_\D8\A8"(i64 %"\D8\B3") {
entry:
  %"\D8\B31" = alloca i64, align 8
  store i64 %"\D8\B3", ptr %"\D8\B31", align 8
  %"%\D8\B3.load" = load i64, ptr %"\D8\B31", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B3.load", 100
  br i1 %cmpgttmp, label %then_3, label %merge_4

merge_4:                                          ; preds = %entry
  %"%\D8\B3.load2" = load i64, ptr %"\D8\B31", align 8
  %calltmp = call i64 @"\D8\AF\D8\A7\D9\84\D8\A9_\D8\A3"(i64 %"%\D8\B3.load2")
  ret i64 %calltmp

then_3:                                           ; preds = %entry
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
  %"%\D8\AE63" = alloca ptr, align 8
  %"%\D8\AC" = alloca i64, align 8
  %jmpbuf43 = alloca [256 x i8], align 16
  %"%\D8\AE" = alloca i64, align 8
  %"%\D9\862" = alloca i64, align 8
  %"%\D9\861" = alloca i64, align 8
  %jmpbuf13 = alloca [256 x i8], align 16
  %"%\D9\85" = alloca ptr, align 8
  %"%\D8\A8" = alloca i1, align 1
  %"%\D8\A3" = alloca i1, align 1
  %jmpbuf = alloca [256 x i8], align 16
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%15.load" = load [256 x i8], ptr %jmpbuf, align 1
  %handler_count = load i32, ptr @__sad_handler_count, align 4
  %handler_slot = getelementptr [64 x ptr], ptr @__sad_handler_stack, i32 0, i32 %handler_count
  store ptr %jmpbuf, ptr %handler_slot, align 8
  %new_count = add i32 %handler_count, 1
  store i32 %new_count, ptr @__sad_handler_count, align 4
  %"%15.load1" = load [256 x i8], ptr %jmpbuf, align 1
  %setjmp_result = call i32 @_setjmp(ptr %jmpbuf, ptr null)
  %setjmp_result64 = sext i32 %setjmp_result to i64
  %cmpnetmp = icmp ne i64 %setjmp_result64, 0
  br i1 %cmpnetmp, label %catch_body_7, label %try_body_6

catch_body_11:                                    ; preds = %try_exit_9
  %handler_count32 = load i32, ptr @__sad_handler_count, align 4
  %new_count33 = sub i32 %handler_count32, 1
  store i32 %new_count33, ptr @__sad_handler_count, align 4
  %exception_msg34 = load ptr, ptr @__sad_exception_msg, align 8
  %obj.ptrtoint = ptrtoint ptr %exception_msg34 to i64
  store i64 %obj.ptrtoint, ptr %"%\D8\AE", align 8
  %"%\D8\AE.load" = load i64, ptr %"%\D8\AE", align 8
  %strbuf35 = alloca i8, i64 32, align 1
  %1 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf35, ptr @int.fmt, i64 %"%\D8\AE.load")
  %len136 = call i64 @strlen(ptr @str.const.16)
  %len237 = call i64 @strlen(ptr %strbuf35)
  %totallen38 = add i64 %len136, %len237
  %bufsize39 = add i64 %totallen38, 1
  %concat.buf40 = call ptr @malloc(i64 %bufsize39)
  %2 = call ptr @memcpy(ptr %concat.buf40, ptr @str.const.16, i64 %len136)
  %dest241 = getelementptr i8, ptr %concat.buf40, i64 %len136
  %copylen242 = add i64 %len237, 1
  %3 = call ptr @memcpy(ptr %dest241, ptr %strbuf35, i64 %copylen242)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf40)
  br label %try_exit_13

catch_body_15:                                    ; preds = %try_exit_13
  %handler_count61 = load i32, ptr @__sad_handler_count, align 4
  %new_count62 = sub i32 %handler_count61, 1
  store i32 %new_count62, ptr @__sad_handler_count, align 4
  %exception_msg64 = load ptr, ptr @__sad_exception_msg, align 8
  store ptr %exception_msg64, ptr %"%\D8\AE63", align 8
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  br label %try_exit_17

catch_body_7:                                     ; preds = %entry
  %handler_count4 = load i32, ptr @__sad_handler_count, align 4
  %new_count5 = sub i32 %handler_count4, 1
  store i32 %new_count5, ptr @__sad_handler_count, align 4
  %exception_msg = load ptr, ptr @__sad_exception_msg, align 8
  store ptr %exception_msg, ptr %"%\D9\85", align 8
  %"%\D9\85.load" = load ptr, ptr %"%\D9\85", align 8
  %len16 = call i64 @strlen(ptr @str.const.7)
  %len27 = call i64 @strlen(ptr %"%\D9\85.load")
  %totallen8 = add i64 %len16, %len27
  %bufsize9 = add i64 %totallen8, 1
  %concat.buf10 = call ptr @malloc(i64 %bufsize9)
  %6 = call ptr @memcpy(ptr %concat.buf10, ptr @str.const.7, i64 %len16)
  %dest211 = getelementptr i8, ptr %concat.buf10, i64 %len16
  %copylen212 = add i64 %len27, 1
  %7 = call ptr @memcpy(ptr %dest211, ptr %"%\D9\85.load", i64 %copylen212)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf10)
  br label %try_exit_9

try_body_10:                                      ; preds = %try_exit_9
  %calltmp = call i64 @"\D8\AF\D8\A7\D9\84\D8\A9_\D8\A8"(i64 5)
  store i64 %calltmp, ptr %"%\D9\861", align 8
  %"%\D9\861.load" = load i64, ptr %"%\D9\861", align 8
  %strbuf21 = alloca i8, i64 32, align 1
  %9 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf21, ptr @int.fmt, i64 %"%\D9\861.load")
  %len122 = call i64 @strlen(ptr @str.const.11)
  %len223 = call i64 @strlen(ptr %strbuf21)
  %totallen24 = add i64 %len122, %len223
  %bufsize25 = add i64 %totallen24, 1
  %concat.buf26 = call ptr @malloc(i64 %bufsize25)
  %10 = call ptr @memcpy(ptr %concat.buf26, ptr @str.const.11, i64 %len122)
  %dest227 = getelementptr i8, ptr %concat.buf26, i64 %len122
  %copylen228 = add i64 %len223, 1
  %11 = call ptr @memcpy(ptr %dest227, ptr %strbuf21, i64 %copylen228)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf26)
  %calltmp29 = call i64 @"\D8\AF\D8\A7\D9\84\D8\A9_\D8\A8"(i64 200)
  store i64 %calltmp29, ptr %"%\D9\862", align 8
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %handler_count30 = load i32, ptr @__sad_handler_count, align 4
  %new_count31 = sub i32 %handler_count30, 1
  store i32 %new_count31, ptr @__sad_handler_count, align 4
  br label %try_exit_13

try_body_14:                                      ; preds = %try_exit_13
  store i64 30, ptr %"%\D8\AC", align 8
  %"%\D8\AC.load" = load i64, ptr %"%\D8\AC", align 8
  %strbuf51 = alloca i8, i64 32, align 1
  %14 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf51, ptr @int.fmt, i64 %"%\D8\AC.load")
  %len152 = call i64 @strlen(ptr @str.const.21)
  %len253 = call i64 @strlen(ptr %strbuf51)
  %totallen54 = add i64 %len152, %len253
  %bufsize55 = add i64 %totallen54, 1
  %concat.buf56 = call ptr @malloc(i64 %bufsize55)
  %15 = call ptr @memcpy(ptr %concat.buf56, ptr @str.const.21, i64 %len152)
  %dest257 = getelementptr i8, ptr %concat.buf56, i64 %len152
  %copylen258 = add i64 %len253, 1
  %16 = call ptr @memcpy(ptr %dest257, ptr %strbuf51, i64 %copylen258)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf56)
  %handler_count59 = load i32, ptr @__sad_handler_count, align 4
  %new_count60 = sub i32 %handler_count59, 1
  store i32 %new_count60, ptr @__sad_handler_count, align 4
  br label %try_exit_17

try_body_6:                                       ; preds = %entry
  store i64 1, ptr %"%\D8\A3", align 8
  %"%\D8\A3.load" = load i1, ptr %"%\D8\A3", align 1
  %strbuf = alloca i8, i64 32, align 1
  %18 = sext i1 %"%\D8\A3.load" to i64
  %19 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf, ptr @int.fmt, i64 %18)
  %len1 = call i64 @strlen(ptr @str.const.3)
  %len2 = call i64 @strlen(ptr %strbuf)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = call ptr @malloc(i64 %bufsize)
  %20 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.3, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %21 = call ptr @memcpy(ptr %dest2, ptr %strbuf, i64 %copylen2)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  store i64 1, ptr %"%\D8\A8", align 8
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %handler_count2 = load i32, ptr @__sad_handler_count, align 4
  %new_count3 = sub i32 %handler_count2, 1
  store i32 %new_count3, ptr @__sad_handler_count, align 4
  br label %try_exit_9

try_exit_13:                                      ; preds = %catch_body_11, %try_body_10
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.19)
  %"%44.load" = load [256 x i8], ptr %jmpbuf43, align 1
  %handler_count44 = load i32, ptr @__sad_handler_count, align 4
  %handler_slot45 = getelementptr [64 x ptr], ptr @__sad_handler_stack, i32 0, i32 %handler_count44
  store ptr %jmpbuf43, ptr %handler_slot45, align 8
  %new_count46 = add i32 %handler_count44, 1
  store i32 %new_count46, ptr @__sad_handler_count, align 4
  %"%44.load47" = load [256 x i8], ptr %jmpbuf43, align 1
  %setjmp_result48 = call i32 @_setjmp(ptr %jmpbuf43, ptr null)
  %setjmp_result6449 = sext i32 %setjmp_result48 to i64
  %cmpnetmp50 = icmp ne i64 %setjmp_result6449, 0
  br i1 %cmpnetmp50, label %catch_body_15, label %try_body_14

try_exit_17:                                      ; preds = %catch_body_15, %try_body_14
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.26)
  ret void

try_exit_9:                                       ; preds = %catch_body_7, %try_body_6
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.9)
  %"%30.load" = load [256 x i8], ptr %jmpbuf13, align 1
  %handler_count14 = load i32, ptr @__sad_handler_count, align 4
  %handler_slot15 = getelementptr [64 x ptr], ptr @__sad_handler_stack, i32 0, i32 %handler_count14
  store ptr %jmpbuf13, ptr %handler_slot15, align 8
  %new_count16 = add i32 %handler_count14, 1
  store i32 %new_count16, ptr @__sad_handler_count, align 4
  %"%30.load17" = load [256 x i8], ptr %jmpbuf13, align 1
  %setjmp_result18 = call i32 @_setjmp(ptr %jmpbuf13, ptr null)
  %setjmp_result6419 = sext i32 %setjmp_result18 to i64
  %cmpnetmp20 = icmp ne i64 %setjmp_result6419, 0
  br i1 %cmpnetmp20, label %catch_body_11, label %try_body_10
}

; Function Attrs: noreturn
declare void @longjmp(ptr, i32) #0

declare i32 @printf(ptr, ...)

; Function Attrs: returns_twice
declare i32 @_setjmp(ptr, ptr) #1

declare i32 @sprintf(ptr, ptr, ...)

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
