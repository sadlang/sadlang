; ModuleID = '03_type_checks'
source_filename = "03_type_checks"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [9 x i8] c"  [\E2\9C\93] \00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.1 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.3 = private unnamed_addr constant [9 x i8] c"  [\E2\9C\97] \00", align 1
@str.const.7 = private unnamed_addr constant [41 x i8] c"=== \D9\81\D8\AD\D8\B5 \D8\AA\D8\B3\D8\A7\D9\88\D9\8A \D8\A7\D9\84\D8\A3\D8\B1\D9\82\D8\A7\D9\85 ===\00", align 1
@str.const.11 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.15 = private unnamed_addr constant [9 x i8] c"10 == 10\00", align 1
@str.const.16 = private unnamed_addr constant [11 x i8] c"5+5 == 2*5\00", align 1
@str.const.17 = private unnamed_addr constant [12 x i8] c"100/4 == 25\00", align 1
@str.const.18 = private unnamed_addr constant [11 x i8] c"0 == 10-10\00", align 1
@str.const.19 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.23 = private unnamed_addr constant [13 x i8] c"  \D9\86\D8\AC\D8\AD\D8\AA: \00", align 1
@str.const.24 = private unnamed_addr constant [14 x i8] c" \D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA\00", align 1
@str.const.27 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.31 = private unnamed_addr constant [39 x i8] c"=== \D9\81\D8\AD\D8\B5 \D8\AA\D8\B3\D8\A7\D9\88\D9\8A \D8\A7\D9\84\D9\86\D8\B5\D9\88\D8\B5 ===\00", align 1
@str.const.35 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.39 = private unnamed_addr constant [11 x i8] c"\D9\85\D8\B1\D8\AD\D8\A8\D8\A7\00", align 1
@str.const.41 = private unnamed_addr constant [25 x i8] c"\D9\85\D8\B1\D8\AD\D8\A8\D8\A7 == \D9\85\D8\B1\D8\AD\D8\A8\D8\A7\00", align 1
@str.const.42 = private unnamed_addr constant [9 x i8] c"\D8\A3\D8\AD\D9\85\D8\AF\00", align 1
@str.const.44 = private unnamed_addr constant [43 x i8] c"\D8\A3\D8\AD\D9\85\D8\AF == \D8\A3\D8\AD\D9\85\D8\AF (\D9\85\D9\86 \D9\85\D8\AA\D8\BA\D9\8A\D8\B1\D8\A7\D8\AA)\00", align 1
@str.const.45 = private unnamed_addr constant [21 x i8] c"\D9\81\D8\B1\D8\A7\D8\BA == \D9\81\D8\B1\D8\A7\D8\BA\00", align 1
@str.const.46 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.54 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.58 = private unnamed_addr constant [28 x i8] c"=== \D9\81\D8\AD\D8\B5 \D8\A7\D9\84\D8\A3\D9\83\D8\A8\D8\B1 ===\00", align 1
@str.const.62 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.66 = private unnamed_addr constant [9 x i8] c"100 > 50\00", align 1
@str.const.67 = private unnamed_addr constant [11 x i8] c"1000 > 999\00", align 1
@str.const.68 = private unnamed_addr constant [11 x i8] c"50*3 > 100\00", align 1
@str.const.69 = private unnamed_addr constant [7 x i8] c"0 > -1\00", align 1
@str.const.70 = private unnamed_addr constant [9 x i8] c"5 > -100\00", align 1
@str.const.71 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.79 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.83 = private unnamed_addr constant [30 x i8] c"=== \D9\81\D8\AD\D8\B5 \D8\A7\D9\84\D8\A3\D9\86\D9\88\D8\A7\D8\B9 ===\00", align 1
@str.const.87 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.91 = private unnamed_addr constant [15 x i8] c"42 \D9\87\D9\88 \D8\B1\D9\82\D9\85\00", align 1
@str.const.93 = private unnamed_addr constant [21 x i8] c"\D9\85\D8\B1\D8\AD\D8\A8\D8\A7 \D9\87\D9\88 \D9\86\D8\B5\00", align 1
@str.const.94 = private unnamed_addr constant [15 x i8] c"99 \D9\87\D9\88 \D8\B1\D9\82\D9\85\00", align 1
@str.const.95 = private unnamed_addr constant [23 x i8] c"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 \D9\87\D9\88 \D9\86\D8\B5\00", align 1
@str.const.96 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.104 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.108 = private unnamed_addr constant [34 x i8] c"=== \D9\81\D8\AD\D8\B5 \D8\A7\D9\84\D8\AA\D8\A3\D9\83\D9\8A\D8\AF\D8\A7\D8\AA ===\00", align 1
@str.const.112 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.116 = private unnamed_addr constant [7 x i8] c"1 == 1\00", align 1
@str.const.117 = private unnamed_addr constant [7 x i8] c"10 > 5\00", align 1
@str.const.118 = private unnamed_addr constant [11 x i8] c"100 != 200\00", align 1
@str.const.119 = private unnamed_addr constant [10 x i8] c"7*7 == 49\00", align 1
@str.const.120 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.128 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.132 = private unnamed_addr constant [109 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.136 = private unnamed_addr constant [65 x i8] c"\E2\95\91  \D8\A7\D9\84\D8\AA\D8\AD\D9\82\D9\82 \D9\85\D9\86 \D8\A7\D9\84\D8\A3\D9\86\D9\88\D8\A7\D8\B9 \D9\88\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D9\88\D8\A7\D8\A9     \E2\95\91\00", align 1
@str.const.140 = private unnamed_addr constant [109 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.144 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.148 = private unnamed_addr constant [43 x i8] c"=== \D8\AC\D9\85\D9\8A\D8\B9 \D8\A7\D9\84\D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA \D9\86\D8\AC\D8\AD\D8\AA ===\00", align 1

define i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\81\D8\AD\D8\B5"(ptr %"\D9\88\D8\B5\D9\81", i64 %"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9") {
entry:
  %"\D9\86\D8\AA\D9\8A\D8\AC\D8\A92" = alloca i64, align 8
  store i64 %"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", ptr %"\D9\86\D8\AA\D9\8A\D8\AC\D8\A92", align 8
  %"\D9\88\D8\B5\D9\811" = alloca ptr, align 8
  store ptr %"\D9\88\D8\B5\D9\81", ptr %"\D9\88\D8\B5\D9\811", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"\D9\86\D8\AA\D9\8A\D8\AC\D8\A92", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 1
  br i1 %cmpeqtmp, label %then_0, label %else_2

then_0:                                           ; preds = %entry
  %len1 = call i64 @strlen(ptr @str.const)
  %len2 = call i64 @strlen(ptr %"\D9\88\D8\B5\D9\811")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D9\88\D8\B5\D9\811", i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_1

else_2:                                           ; preds = %entry
  %len13 = call i64 @strlen(ptr @str.const.3)
  %len24 = call i64 @strlen(ptr %"\D9\88\D8\B5\D9\811")
  %totallen5 = add i64 %len13, %len24
  %bufsize6 = add i64 %totallen5, 1
  %concat.buf7 = alloca i8, i64 %bufsize6, align 1
  %4 = call ptr @memcpy(ptr %concat.buf7, ptr @str.const.3, i64 %len13)
  %dest28 = getelementptr i8, ptr %concat.buf7, i64 %len13
  %copylen29 = add i64 %len24, 1
  %5 = call ptr @memcpy(ptr %dest28, ptr %"\D9\88\D8\B5\D9\811", i64 %copylen29)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf7)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_1

merge_1:                                          ; preds = %else_2, %then_0
  ret i64 0
}

define i64 @"\D9\81\D8\AD\D8\B5_\D9\85\D8\B3\D8\A7\D9\88\D8\A7\D8\A9_\D8\A3\D8\B1\D9\82\D8\A7\D9\85"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.11)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  br i1 true, label %aeq.pass, label %aeq.fail

aeq.fail:                                         ; preds = %entry
  call void @abort()
  unreachable

aeq.pass:                                         ; preds = %entry
  %"%\D8\B9\D8\AF\D8\A7\D8\AF.load" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D8\B9\D8\AF\D8\A7\D8\AF.load", 1
  store i64 %addtmp, ptr %alloca, align 8
  %calltmp = call i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\81\D8\AD\D8\B5"(ptr @str.const.15, i64 1)
  %alloca1 = alloca i64, align 8
  store i64 10, ptr %alloca1, align 8
  %alloca2 = alloca i64, align 8
  store i64 10, ptr %alloca2, align 8
  %"%\D8\A3.load" = load i64, ptr %alloca1, align 8
  %"%\D8\A8.load" = load i64, ptr %alloca2, align 8
  %assert.eq = icmp eq i64 %"%\D8\A3.load", %"%\D8\A8.load"
  br i1 %assert.eq, label %aeq.pass4, label %aeq.fail3

aeq.fail3:                                        ; preds = %aeq.pass
  call void @abort()
  unreachable

aeq.pass4:                                        ; preds = %aeq.pass
  %"%\D8\B9\D8\AF\D8\A7\D8\AF.load5" = load i64, ptr %alloca, align 8
  %addtmp6 = add i64 %"%\D8\B9\D8\AF\D8\A7\D8\AF.load5", 1
  store i64 %addtmp6, ptr %alloca, align 8
  %calltmp7 = call i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\81\D8\AD\D8\B5"(ptr @str.const.16, i64 1)
  %alloca8 = alloca i64, align 8
  store i64 25, ptr %alloca8, align 8
  %alloca9 = alloca i64, align 8
  store i64 25, ptr %alloca9, align 8
  %"%\D8\AC.load" = load i64, ptr %alloca8, align 8
  %"%\D8\AF.load" = load i64, ptr %alloca9, align 8
  %assert.eq10 = icmp eq i64 %"%\D8\AC.load", %"%\D8\AF.load"
  br i1 %assert.eq10, label %aeq.pass12, label %aeq.fail11

aeq.fail11:                                       ; preds = %aeq.pass4
  call void @abort()
  unreachable

aeq.pass12:                                       ; preds = %aeq.pass4
  %"%\D8\B9\D8\AF\D8\A7\D8\AF.load13" = load i64, ptr %alloca, align 8
  %addtmp14 = add i64 %"%\D8\B9\D8\AF\D8\A7\D8\AF.load13", 1
  store i64 %addtmp14, ptr %alloca, align 8
  %calltmp15 = call i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\81\D8\AD\D8\B5"(ptr @str.const.17, i64 1)
  %alloca16 = alloca i64, align 8
  store i64 0, ptr %alloca16, align 8
  %alloca17 = alloca i64, align 8
  store i64 0, ptr %alloca17, align 8
  %"%\D8\B5\D8\B5.load" = load i64, ptr %alloca16, align 8
  %"%\D8\B2\D8\B2.load" = load i64, ptr %alloca17, align 8
  %assert.eq18 = icmp eq i64 %"%\D8\B5\D8\B5.load", %"%\D8\B2\D8\B2.load"
  br i1 %assert.eq18, label %aeq.pass20, label %aeq.fail19

aeq.fail19:                                       ; preds = %aeq.pass12
  call void @abort()
  unreachable

aeq.pass20:                                       ; preds = %aeq.pass12
  %"%\D8\B9\D8\AF\D8\A7\D8\AF.load21" = load i64, ptr %alloca, align 8
  %addtmp22 = add i64 %"%\D8\B9\D8\AF\D8\A7\D8\AF.load21", 1
  store i64 %addtmp22, ptr %alloca, align 8
  %calltmp23 = call i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\81\D8\AD\D8\B5"(ptr @str.const.18, i64 1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.19)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.27)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D9\81\D8\AD\D8\B5_\D9\85\D8\B3\D8\A7\D9\88\D8\A7\D8\A9_\D9\86\D8\B5\D9\88\D8\B5"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.31)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.35)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  call void @sad_security_assert_equal_str(ptr @str.const.39, ptr @str.const.39)
  %"%\D8\B9\D8\AF\D8\A7\D8\AF.load" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D8\B9\D8\AF\D8\A7\D8\AF.load", 1
  store i64 %addtmp, ptr %alloca, align 8
  %calltmp = call i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\81\D8\AD\D8\B5"(ptr @str.const.41, i64 1)
  %alloca1 = alloca ptr, align 8
  store ptr @str.const.42, ptr %alloca1, align 8
  %alloca2 = alloca ptr, align 8
  store ptr @str.const.42, ptr %alloca2, align 8
  call void @sad_security_assert_equal_str(ptr %alloca1, ptr %alloca2)
  %"%\D8\B9\D8\AF\D8\A7\D8\AF.load3" = load i64, ptr %alloca, align 8
  %addtmp4 = add i64 %"%\D8\B9\D8\AF\D8\A7\D8\AF.load3", 1
  store i64 %addtmp4, ptr %alloca, align 8
  %calltmp5 = call i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\81\D8\AD\D8\B5"(ptr @str.const.44, i64 1)
  %alloca6 = alloca ptr, align 8
  %alloca7 = alloca ptr, align 8
  call void @sad_security_assert_equal_str(ptr %alloca6, ptr %alloca7)
  %"%\D8\B9\D8\AF\D8\A7\D8\AF.load8" = load i64, ptr %alloca, align 8
  %addtmp9 = add i64 %"%\D8\B9\D8\AF\D8\A7\D8\AF.load8", 1
  store i64 %addtmp9, ptr %alloca, align 8
  %calltmp10 = call i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\81\D8\AD\D8\B5"(ptr @str.const.45, i64 1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.46)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.54)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D9\81\D8\AD\D8\B5_\D8\A3\D9\83\D8\A8\D8\B1"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.58)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.62)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  br i1 true, label %agt.pass, label %agt.fail

agt.fail:                                         ; preds = %entry
  call void @abort()
  unreachable

agt.pass:                                         ; preds = %entry
  %"%\D8\B9\D8\AF\D8\A7\D8\AF.load" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D8\B9\D8\AF\D8\A7\D8\AF.load", 1
  store i64 %addtmp, ptr %alloca, align 8
  %calltmp = call i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\81\D8\AD\D8\B5"(ptr @str.const.66, i64 1)
  br i1 true, label %agt.pass2, label %agt.fail1

agt.fail1:                                        ; preds = %agt.pass
  call void @abort()
  unreachable

agt.pass2:                                        ; preds = %agt.pass
  %"%\D8\B9\D8\AF\D8\A7\D8\AF.load3" = load i64, ptr %alloca, align 8
  %addtmp4 = add i64 %"%\D8\B9\D8\AF\D8\A7\D8\AF.load3", 1
  store i64 %addtmp4, ptr %alloca, align 8
  %calltmp5 = call i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\81\D8\AD\D8\B5"(ptr @str.const.67, i64 1)
  %alloca6 = alloca i64, align 8
  store i64 150, ptr %alloca6, align 8
  %alloca7 = alloca i64, align 8
  store i64 100, ptr %alloca7, align 8
  %"%\D8\B6.load" = load i64, ptr %alloca6, align 8
  %"%\D8\B7.load" = load i64, ptr %alloca7, align 8
  %assert.gt = icmp sgt i64 %"%\D8\B6.load", %"%\D8\B7.load"
  br i1 %assert.gt, label %agt.pass9, label %agt.fail8

agt.fail8:                                        ; preds = %agt.pass2
  call void @abort()
  unreachable

agt.pass9:                                        ; preds = %agt.pass2
  %"%\D8\B9\D8\AF\D8\A7\D8\AF.load10" = load i64, ptr %alloca, align 8
  %addtmp11 = add i64 %"%\D8\B9\D8\AF\D8\A7\D8\AF.load10", 1
  store i64 %addtmp11, ptr %alloca, align 8
  %calltmp12 = call i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\81\D8\AD\D8\B5"(ptr @str.const.68, i64 1)
  br i1 true, label %agt.pass14, label %agt.fail13

agt.fail13:                                       ; preds = %agt.pass9
  call void @abort()
  unreachable

agt.pass14:                                       ; preds = %agt.pass9
  %"%\D8\B9\D8\AF\D8\A7\D8\AF.load15" = load i64, ptr %alloca, align 8
  %addtmp16 = add i64 %"%\D8\B9\D8\AF\D8\A7\D8\AF.load15", 1
  store i64 %addtmp16, ptr %alloca, align 8
  %calltmp17 = call i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\81\D8\AD\D8\B5"(ptr @str.const.69, i64 1)
  br i1 true, label %agt.pass19, label %agt.fail18

agt.fail18:                                       ; preds = %agt.pass14
  call void @abort()
  unreachable

agt.pass19:                                       ; preds = %agt.pass14
  %"%\D8\B9\D8\AF\D8\A7\D8\AF.load20" = load i64, ptr %alloca, align 8
  %addtmp21 = add i64 %"%\D8\B9\D8\AF\D8\A7\D8\AF.load20", 1
  store i64 %addtmp21, ptr %alloca, align 8
  %calltmp22 = call i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\81\D8\AD\D8\B5"(ptr @str.const.70, i64 1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.71)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.79)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D9\81\D8\AD\D8\B5_\D8\A3\D9\86\D9\88\D8\A7\D8\B9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.83)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.87)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 42, ptr %alloca1, align 8
  %"%\D8\B9\D8\AF\D8\A7\D8\AF.load" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D8\B9\D8\AF\D8\A7\D8\AF.load", 1
  store i64 %addtmp, ptr %alloca, align 8
  %calltmp = call i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\81\D8\AD\D8\B5"(ptr @str.const.91, i64 1)
  %alloca2 = alloca ptr, align 8
  store ptr @str.const.39, ptr %alloca2, align 8
  %"%\D8\B9\D8\AF\D8\A7\D8\AF.load3" = load i64, ptr %alloca, align 8
  %addtmp4 = add i64 %"%\D8\B9\D8\AF\D8\A7\D8\AF.load3", 1
  store i64 %addtmp4, ptr %alloca, align 8
  %calltmp5 = call i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\81\D8\AD\D8\B5"(ptr @str.const.93, i64 1)
  %"%\D8\B9\D8\AF\D8\A7\D8\AF.load6" = load i64, ptr %alloca, align 8
  %addtmp7 = add i64 %"%\D8\B9\D8\AF\D8\A7\D8\AF.load6", 1
  store i64 %addtmp7, ptr %alloca, align 8
  %calltmp8 = call i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\81\D8\AD\D8\B5"(ptr @str.const.94, i64 1)
  %"%\D8\B9\D8\AF\D8\A7\D8\AF.load9" = load i64, ptr %alloca, align 8
  %addtmp10 = add i64 %"%\D8\B9\D8\AF\D8\A7\D8\AF.load9", 1
  store i64 %addtmp10, ptr %alloca, align 8
  %calltmp11 = call i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\81\D8\AD\D8\B5"(ptr @str.const.95, i64 1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.96)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.104)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D9\81\D8\AD\D8\B5_\D8\AA\D8\A3\D9\83\D9\8A\D8\AF\D8\A7\D8\AA"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.108)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.112)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  br i1 true, label %sec.pass, label %sec.fail

sec.fail:                                         ; preds = %entry
  call void @abort()
  unreachable

sec.pass:                                         ; preds = %entry
  %"%\D8\B9\D8\AF\D8\A7\D8\AF.load" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D8\B9\D8\AF\D8\A7\D8\AF.load", 1
  store i64 %addtmp, ptr %alloca, align 8
  %calltmp = call i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\81\D8\AD\D8\B5"(ptr @str.const.116, i64 1)
  br i1 true, label %sec.pass2, label %sec.fail1

sec.fail1:                                        ; preds = %sec.pass
  call void @abort()
  unreachable

sec.pass2:                                        ; preds = %sec.pass
  %"%\D8\B9\D8\AF\D8\A7\D8\AF.load3" = load i64, ptr %alloca, align 8
  %addtmp4 = add i64 %"%\D8\B9\D8\AF\D8\A7\D8\AF.load3", 1
  store i64 %addtmp4, ptr %alloca, align 8
  %calltmp5 = call i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\81\D8\AD\D8\B5"(ptr @str.const.117, i64 1)
  br i1 true, label %sec.pass7, label %sec.fail6

sec.fail6:                                        ; preds = %sec.pass2
  call void @abort()
  unreachable

sec.pass7:                                        ; preds = %sec.pass2
  %"%\D8\B9\D8\AF\D8\A7\D8\AF.load8" = load i64, ptr %alloca, align 8
  %addtmp9 = add i64 %"%\D8\B9\D8\AF\D8\A7\D8\AF.load8", 1
  store i64 %addtmp9, ptr %alloca, align 8
  %calltmp10 = call i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\81\D8\AD\D8\B5"(ptr @str.const.118, i64 1)
  %alloca11 = alloca i64, align 8
  store i64 49, ptr %alloca11, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %alloca11, align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 49
  %4 = icmp ne i1 %cmpeqtmp, false
  br i1 %4, label %sec.pass13, label %sec.fail12

sec.fail12:                                       ; preds = %sec.pass7
  call void @abort()
  unreachable

sec.pass13:                                       ; preds = %sec.pass7
  %"%\D8\B9\D8\AF\D8\A7\D8\AF.load14" = load i64, ptr %alloca, align 8
  %addtmp15 = add i64 %"%\D8\B9\D8\AF\D8\A7\D8\AF.load14", 1
  store i64 %addtmp15, ptr %alloca, align 8
  %calltmp16 = call i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\81\D8\AD\D8\B5"(ptr @str.const.119, i64 1)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.120)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.128)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.132)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.136)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.140)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.144)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call i64 @"\D9\81\D8\AD\D8\B5_\D9\85\D8\B3\D8\A7\D9\88\D8\A7\D8\A9_\D8\A3\D8\B1\D9\82\D8\A7\D9\85"()
  %calltmp1 = call i64 @"\D9\81\D8\AD\D8\B5_\D9\85\D8\B3\D8\A7\D9\88\D8\A7\D8\A9_\D9\86\D8\B5\D9\88\D8\B5"()
  %calltmp2 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\A3\D9\83\D8\A8\D8\B1"()
  %calltmp3 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\A3\D9\86\D9\88\D8\A7\D8\B9"()
  %calltmp4 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\AA\D8\A3\D9\83\D9\8A\D8\AF\D8\A7\D8\AA"()
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.148)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

declare i64 @strlen(ptr)

declare ptr @memcpy(ptr, ptr, i64)

declare i32 @printf(ptr, ...)

declare void @abort()

declare void @sad_security_assert_equal_str(ptr, ptr)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
