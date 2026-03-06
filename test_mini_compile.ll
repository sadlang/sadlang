; ModuleID = 'test_mini_compile'
source_filename = "test_mini_compile"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [11 x i8] c"\D9\85\D8\B1\D8\AD\D8\A8\D8\A7\00", align 1
@fmt.s = private unnamed_addr constant [3 x i8] c"%s\00", align 1

define void @__sad_main() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  ret void
}

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
