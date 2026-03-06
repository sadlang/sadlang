; Test setjmp/longjmp with lli
declare i32 @_setjmp(ptr, ptr)
declare void @longjmp(ptr, i32)
declare i32 @printf(ptr, ...)

@fmt1 = private unnamed_addr constant [21 x i8] c"setjmp returned: %d\0A\00"
@fmt2 = private unnamed_addr constant [19 x i8] c"caught exception!\0A\00"
@fmt3 = private unnamed_addr constant [11 x i8] c"try block\0A\00"

define i32 @main() {
entry:
  %buf = alloca [256 x i8], align 16
  %result = call i32 @_setjmp(ptr %buf, ptr null)
  %fmt_ptr = getelementptr inbounds [21 x i8], ptr @fmt1, i32 0, i32 0
  call i32 (ptr, ...) @printf(ptr %fmt_ptr, i32 %result)
  %is_exc = icmp ne i32 %result, 0
  br i1 %is_exc, label %catch, label %try_body

try_body:
  %fmt3_ptr = getelementptr inbounds [11 x i8], ptr @fmt3, i32 0, i32 0
  call i32 (ptr, ...) @printf(ptr %fmt3_ptr)
  ; simulate throw
  call void @longjmp(ptr %buf, i32 1)
  unreachable

catch:
  %fmt2_ptr = getelementptr inbounds [19 x i8], ptr @fmt2, i32 0, i32 0
  call i32 (ptr, ...) @printf(ptr %fmt2_ptr)
  ret i32 0
}
