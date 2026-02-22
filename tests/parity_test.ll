; ModuleID = 'parity_test'
source_filename = "parity_test"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@"\D8\B3" = global i64 10
@"\D8\B9" = global i64 20
@"\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9" = global i64 0
@"\D9\85\D8\AC\D9\85\D9\88\D8\B9" = global i64 0
@"\D8\AA" = global i64 1
@"\D8\A3\D8\B1\D9\82\D8\A7\D9\85" = global i64 0
@"\D8\A7\D8\B3\D9\85" = global i64 0

define i64 @"\D9\85\D8\B6\D8\B1\D9\88\D8\A8"(i64 %"\D9\86") {
entry:
  %"\D9\861" = alloca i64, align 8
  store i64 %"\D9\86", ptr %"\D9\861", align 8
  %"%\D9\86.load" = load i64, ptr %"\D9\861", align 8
  %cmpletmp = icmp sle i64 %"%\D9\86.load", 1
  br i1 %cmpletmp, label %then_0, label %merge_1

merge_1:                                          ; preds = %entry
  %"%\D9\86.load2" = load i64, ptr %"\D9\861", align 8
  %subtmp = sub i64 %"%\D9\86.load2", 1
  %calltmp = call i64 @"\D9\85\D8\B6\D8\B1\D9\88\D8\A8"(i64 %subtmp)
  %"%\D9\86.load3" = load i64, ptr %"\D9\861", align 8
  %multmp = mul i64 %"%\D9\86.load3", %calltmp
  ret i64 %multmp

then_0:                                           ; preds = %entry
  ret i64 1
}
