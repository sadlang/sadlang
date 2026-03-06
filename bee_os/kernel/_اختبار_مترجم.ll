; ModuleID = '_اختبار_مترجم'
source_filename = "_\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D9\85\D8\AA\D8\B1\D8\AC\D9\85"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@"\D8\AD\D8\AF_\D8\A3\D8\B9\D9\84\D9\89" = constant i64 1024
@"\D8\AE\D8\A7\D9\86\D8\A9_0" = global i64 0
@"\D8\AE\D8\A7\D9\86\D8\A9_1" = global i64 0
@"\D8\AE\D8\A7\D9\86\D8\A9_2" = global i64 0
@"\D8\AE\D8\A7\D9\86\D8\A9_3" = global i64 0
@str.const = private unnamed_addr constant [35 x i8] c"  [test] compiler features test OK\00", align 1

declare void @kernel_print_string(i64)

declare void @kernel_print_number(i64)

define i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D9\86\D8\B3\D8\AE"(i64 %"\D9\82\D9\8A\D9\85\D8\A9") {
entry:
  %"%\D9\86\D8\B3\D8\AE\D8\A9" = alloca i64, align 8
  %"\D9\82\D9\8A\D9\85\D8\A91" = alloca i64, align 8
  store i64 %"\D9\82\D9\8A\D9\85\D8\A9", ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %"%1.load" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  store i64 %"%1.load", ptr %"%\D9\86\D8\B3\D8\AE\D8\A9", align 8
  %loadtmp = load i64, ptr %"%\D9\86\D8\B3\D8\AE\D8\A9", align 8
  ret i64 %loadtmp
}

define i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D8\B4\D8\B1\D9\88\D8\B7"(i64 %"\D8\A3", i64 %"\D8\A8") {
entry:
  %"\D8\A82" = alloca i64, align 8
  store i64 %"\D8\A8", ptr %"\D8\A82", align 8
  %"\D8\A31" = alloca i64, align 8
  store i64 %"\D8\A3", ptr %"\D8\A31", align 8
  %"%\D8\A3.load" = load i64, ptr %"\D8\A31", align 8
  %"%\D8\A8.load" = load i64, ptr %"\D8\A82", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\A3.load", %"%\D8\A8.load"
  br i1 %cmpgttmp, label %then_0, label %else_2

else_2:                                           ; preds = %entry
  %"%\D8\A3.load3" = load i64, ptr %"\D8\A31", align 8
  %"%\D8\A8.load4" = load i64, ptr %"\D8\A82", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\A3.load3", %"%\D8\A8.load4"
  br i1 %cmpeqtmp, label %then_3, label %else_5

else_5:                                           ; preds = %else_2
  ret i64 -1

then_0:                                           ; preds = %entry
  ret i64 1

then_3:                                           ; preds = %else_2
  ret i64 0
}

define i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D8\AD\D9\84\D9\82\D8\A9"(i64 %"\D8\B9\D8\AF\D8\AF") {
entry:
  %"%\D8\B9" = alloca i64, align 8
  %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9" = alloca i64, align 8
  %"\D8\B9\D8\AF\D8\AF1" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF", ptr %"\D8\B9\D8\AF\D8\AF1", align 8
  store i64 0, ptr %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9", align 8
  store i64 0, ptr %"%\D8\B9", align 8
  br label %while_cond_6

while_body_7:                                     ; preds = %while_cond_6
  %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9.load" = load i64, ptr %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9", align 8
  %"%\D8\B9.load2" = load i64, ptr %"%\D8\B9", align 8
  %addtmp = add i64 %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9.load", %"%\D8\B9.load2"
  store i64 %addtmp, ptr %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9", align 8
  %"%\D8\B9.load3" = load i64, ptr %"%\D8\B9", align 8
  %addtmp4 = add i64 %"%\D8\B9.load3", 1
  store i64 %addtmp4, ptr %"%\D8\B9", align 8
  br label %while_cond_6

while_cond_6:                                     ; preds = %while_body_7, %entry
  %"%\D8\B9.load" = load i64, ptr %"%\D8\B9", align 8
  %"%\D8\B9\D8\AF\D8\AF.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF1", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B9.load", %"%\D8\B9\D8\AF\D8\AF.load"
  br i1 %cmplttmp, label %while_body_7, label %while_exit_8

while_exit_8:                                     ; preds = %while_cond_6
  %loadtmp = load i64, ptr %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9", align 8
  ret i64 %loadtmp
}

define i64 @"\D8\A7\D8\B3\D8\AA\D8\AE\D8\B1\D8\AC_\D8\B9\D8\B1\D8\B6"(i64 %"\D9\82\D9\8A\D9\85\D8\A9", i64 %"\D9\85\D9\88\D8\B6\D8\B9") {
entry:
  %"%\D8\A7\D9\84\D8\A8\D8\A7\D9\82\D9\8A" = alloca i64, align 8
  %"%\D8\B9" = alloca i64, align 8
  %"%\D9\85\D9\82\D8\B3\D9\88\D9\85" = alloca i64, align 8
  %"\D9\85\D9\88\D8\B6\D8\B92" = alloca i64, align 8
  store i64 %"\D9\85\D9\88\D8\B6\D8\B9", ptr %"\D9\85\D9\88\D8\B6\D8\B92", align 8
  %"\D9\82\D9\8A\D9\85\D8\A91" = alloca i64, align 8
  store i64 %"\D9\82\D9\8A\D9\85\D8\A9", ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %"%18.load" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  store i64 %"%18.load", ptr %"%\D9\85\D9\82\D8\B3\D9\88\D9\85", align 8
  store i64 0, ptr %"%\D8\B9", align 8
  br label %while_cond_9

while_body_10:                                    ; preds = %while_cond_9
  %"%\D9\85\D9\82\D8\B3\D9\88\D9\85.load" = load i64, ptr %"%\D9\85\D9\82\D8\B3\D9\88\D9\85", align 8
  %shrtmp = lshr i64 %"%\D9\85\D9\82\D8\B3\D9\88\D9\85.load", 8
  store i64 %shrtmp, ptr %"%\D9\85\D9\82\D8\B3\D9\88\D9\85", align 8
  %"%\D8\B9.load3" = load i64, ptr %"%\D8\B9", align 8
  %addtmp = add i64 %"%\D8\B9.load3", 1
  store i64 %addtmp, ptr %"%\D8\B9", align 8
  br label %while_cond_9

while_cond_9:                                     ; preds = %while_body_10, %entry
  %"%\D8\B9.load" = load i64, ptr %"%\D8\B9", align 8
  %"%\D9\85\D9\88\D8\B6\D8\B9.load" = load i64, ptr %"\D9\85\D9\88\D8\B6\D8\B92", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B9.load", %"%\D9\85\D9\88\D8\B6\D8\B9.load"
  br i1 %cmplttmp, label %while_body_10, label %while_exit_11

while_exit_11:                                    ; preds = %while_cond_9
  %"%\D9\85\D9\82\D8\B3\D9\88\D9\85.load4" = load i64, ptr %"%\D9\85\D9\82\D8\B3\D9\88\D9\85", align 8
  %shrtmp5 = lshr i64 %"%\D9\85\D9\82\D8\B3\D9\88\D9\85.load4", 8
  %shltmp = shl i64 %shrtmp5, 8
  %"%\D9\85\D9\82\D8\B3\D9\88\D9\85.load6" = load i64, ptr %"%\D9\85\D9\82\D8\B3\D9\88\D9\85", align 8
  %subtmp = sub i64 %"%\D9\85\D9\82\D8\B3\D9\88\D9\85.load6", %shltmp
  store i64 %subtmp, ptr %"%\D8\A7\D9\84\D8\A8\D8\A7\D9\82\D9\8A", align 8
  %loadtmp = load i64, ptr %"%\D8\A7\D9\84\D8\A8\D8\A7\D9\82\D9\8A", align 8
  ret i64 %loadtmp
}

define void @__sad_init_99() {
entry:
  call void @kernel_print_string(i64 ptrtoint (ptr @str.const to i64))
  call void @kernel_print_number(i64 10)
  ret void
}

define void @__sad_main() {
entry:
  %"%\D8\AE\D8\A7\D9\86\D8\A9_3" = alloca i64, align 8
  %"%\D8\AE\D8\A7\D9\86\D8\A9_2" = alloca i64, align 8
  %"%\D8\AE\D8\A7\D9\86\D8\A9_1" = alloca i64, align 8
  %"%\D8\AE\D8\A7\D9\86\D8\A9_0" = alloca i64, align 8
  %"%\D8\AD\D8\AF_\D8\A3\D8\B9\D9\84\D9\89" = alloca i64, align 8
  store i64 1024, ptr %"%\D8\AD\D8\AF_\D8\A3\D8\B9\D9\84\D9\89", align 8
  store i64 0, ptr %"%\D8\AE\D8\A7\D9\86\D8\A9_0", align 8
  store i64 0, ptr %"%\D8\AE\D8\A7\D9\86\D8\A9_1", align 8
  store i64 0, ptr %"%\D8\AE\D8\A7\D9\86\D8\A9_2", align 8
  store i64 0, ptr %"%\D8\AE\D8\A7\D9\86\D8\A9_3", align 8
  ret void
}

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
