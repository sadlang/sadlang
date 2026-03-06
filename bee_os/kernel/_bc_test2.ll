; ModuleID = '_bc_test2'
source_filename = "_bc_test2"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [3 x i8] c"ok\00", align 1

declare void @kernel_print_string(i64)

define void @__sad_init_99() {
entry:
  call void @kernel_print_string(i64 ptrtoint (ptr @str.const to i64))
  ret void
}
