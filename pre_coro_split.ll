; ModuleID = 'test_gen_sadc'
source_filename = "test_gen_sadc"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@fmt.d = private unnamed_addr constant [5 x i8] c"%lld\00", align 1

; Function Attrs: presplitcoroutine
define ptr @"\D8\A8\D8\B3\D9\8A\D8\B7"() #0 {
entry:
  %coro.promise = alloca i64, align 8
  store i64 0, ptr %coro.promise, align 8
  %coro.id = call token @llvm.coro.id(i32 0, ptr %coro.promise, ptr null, ptr null)
  %coro.size = call i64 @llvm.coro.size.i64()
  %coro.alloc = call ptr @malloc(i64 %coro.size)
  %coro.hdl = call ptr @llvm.coro.begin(token %coro.id, ptr %coro.alloc)
  %coro.init.save = call token @llvm.coro.save(ptr %coro.hdl)
  %coro.init.susp = call i8 @llvm.coro.suspend(token %coro.init.save, i1 false)
  switch i8 %coro.init.susp, label %coro.suspend [
    i8 0, label %coro.init.resume
    i8 1, label %coro.cleanup
  ]

coro.final:                                       ; preds = %gen.yield.0.resume
  %coro.final.save = call token @llvm.coro.save(ptr %coro.hdl)
  %coro.final.susp = call i8 @llvm.coro.suspend(token %coro.final.save, i1 true)
  switch i8 %coro.final.susp, label %coro.suspend [
    i8 0, label %coro.suspend
    i8 1, label %coro.cleanup
  ]

coro.cleanup:                                     ; preds = %coro.final, %coro.init.resume, %entry
  %coro.mem = call ptr @llvm.coro.free(token %coro.id, ptr %coro.hdl)
  call void @free(ptr %coro.mem)
  br label %coro.suspend

coro.suspend:                                     ; preds = %coro.cleanup, %coro.final, %coro.final, %coro.init.resume, %entry
  %0 = call i1 @llvm.coro.end(ptr %coro.hdl, i1 false, token none)
  ret ptr %coro.hdl

coro.init.resume:                                 ; preds = %entry
  store i64 42, ptr %coro.promise, align 8
  %gen.yield.0.save = call token @llvm.coro.save(ptr %coro.hdl)
  %gen.yield.0.susp = call i8 @llvm.coro.suspend(token %gen.yield.0.save, i1 false)
  switch i8 %gen.yield.0.susp, label %coro.suspend [
    i8 0, label %gen.yield.0.resume
    i8 1, label %coro.cleanup
  ]

gen.yield.0.resume:                               ; preds = %coro.init.resume
  br label %coro.final
}

; Function Attrs: presplitcoroutine
define ptr @"\D8\B6\D8\B9\D9\81"(i64 %"\D9\86") #0 {
entry:
  %"\D9\861" = alloca i64, align 8
  store i64 %"\D9\86", ptr %"\D9\861", align 8
  %coro.promise = alloca i64, align 8
  store i64 0, ptr %coro.promise, align 8
  %coro.id = call token @llvm.coro.id(i32 0, ptr %coro.promise, ptr null, ptr null)
  %coro.size = call i64 @llvm.coro.size.i64()
  %coro.alloc = call ptr @malloc(i64 %coro.size)
  %coro.hdl = call ptr @llvm.coro.begin(token %coro.id, ptr %coro.alloc)
  %coro.init.save = call token @llvm.coro.save(ptr %coro.hdl)
  %coro.init.susp = call i8 @llvm.coro.suspend(token %coro.init.save, i1 false)
  switch i8 %coro.init.susp, label %coro.suspend [
    i8 0, label %coro.init.resume
    i8 1, label %coro.cleanup
  ]

coro.final:                                       ; preds = %gen.yield.0.resume
  %coro.final.save = call token @llvm.coro.save(ptr %coro.hdl)
  %coro.final.susp = call i8 @llvm.coro.suspend(token %coro.final.save, i1 true)
  switch i8 %coro.final.susp, label %coro.suspend [
    i8 0, label %coro.suspend
    i8 1, label %coro.cleanup
  ]

coro.cleanup:                                     ; preds = %coro.final, %coro.init.resume, %entry
  %coro.mem = call ptr @llvm.coro.free(token %coro.id, ptr %coro.hdl)
  call void @free(ptr %coro.mem)
  br label %coro.suspend

coro.suspend:                                     ; preds = %coro.cleanup, %coro.final, %coro.final, %coro.init.resume, %entry
  %0 = call i1 @llvm.coro.end(ptr %coro.hdl, i1 false, token none)
  ret ptr %coro.hdl

coro.init.resume:                                 ; preds = %entry
  %"%\D9\86.load" = load i64, ptr %"\D9\861", align 8
  %shltmp = shl i64 %"%\D9\86.load", 1
  store i64 %shltmp, ptr %coro.promise, align 8
  %gen.yield.0.save = call token @llvm.coro.save(ptr %coro.hdl)
  %gen.yield.0.susp = call i8 @llvm.coro.suspend(token %gen.yield.0.save, i1 false)
  switch i8 %gen.yield.0.susp, label %coro.suspend [
    i8 0, label %gen.yield.0.resume
    i8 1, label %coro.cleanup
  ]

gen.yield.0.resume:                               ; preds = %coro.init.resume
  br label %coro.final
}

; Function Attrs: presplitcoroutine
define ptr @"\D9\85\D8\AC\D9\85\D9\88\D8\B9"() #0 {
entry:
  %coro.promise = alloca i64, align 8
  store i64 0, ptr %coro.promise, align 8
  %coro.id = call token @llvm.coro.id(i32 0, ptr %coro.promise, ptr null, ptr null)
  %coro.size = call i64 @llvm.coro.size.i64()
  %coro.alloc = call ptr @malloc(i64 %coro.size)
  %coro.hdl = call ptr @llvm.coro.begin(token %coro.id, ptr %coro.alloc)
  %coro.init.save = call token @llvm.coro.save(ptr %coro.hdl)
  %coro.init.susp = call i8 @llvm.coro.suspend(token %coro.init.save, i1 false)
  switch i8 %coro.init.susp, label %coro.suspend [
    i8 0, label %coro.init.resume
    i8 1, label %coro.cleanup
  ]

coro.final:                                       ; preds = %gen.yield.2.resume
  %coro.final.save = call token @llvm.coro.save(ptr %coro.hdl)
  %coro.final.susp = call i8 @llvm.coro.suspend(token %coro.final.save, i1 true)
  switch i8 %coro.final.susp, label %coro.suspend [
    i8 0, label %coro.suspend
    i8 1, label %coro.cleanup
  ]

coro.cleanup:                                     ; preds = %coro.final, %gen.yield.1.resume, %gen.yield.0.resume, %coro.init.resume, %entry
  %coro.mem = call ptr @llvm.coro.free(token %coro.id, ptr %coro.hdl)
  call void @free(ptr %coro.mem)
  br label %coro.suspend

coro.suspend:                                     ; preds = %coro.cleanup, %coro.final, %coro.final, %gen.yield.1.resume, %gen.yield.0.resume, %coro.init.resume, %entry
  %0 = call i1 @llvm.coro.end(ptr %coro.hdl, i1 false, token none)
  ret ptr %coro.hdl

coro.init.resume:                                 ; preds = %entry
  store i64 10, ptr %coro.promise, align 8
  %gen.yield.0.save = call token @llvm.coro.save(ptr %coro.hdl)
  %gen.yield.0.susp = call i8 @llvm.coro.suspend(token %gen.yield.0.save, i1 false)
  switch i8 %gen.yield.0.susp, label %coro.suspend [
    i8 0, label %gen.yield.0.resume
    i8 1, label %coro.cleanup
  ]

gen.yield.0.resume:                               ; preds = %coro.init.resume
  store i64 20, ptr %coro.promise, align 8
  %gen.yield.1.save = call token @llvm.coro.save(ptr %coro.hdl)
  %gen.yield.1.susp = call i8 @llvm.coro.suspend(token %gen.yield.1.save, i1 false)
  switch i8 %gen.yield.1.susp, label %coro.suspend [
    i8 0, label %gen.yield.1.resume
    i8 1, label %coro.cleanup
  ]

gen.yield.1.resume:                               ; preds = %gen.yield.0.resume
  store i64 30, ptr %coro.promise, align 8
  %gen.yield.2.save = call token @llvm.coro.save(ptr %coro.hdl)
  %gen.yield.2.susp = call i8 @llvm.coro.suspend(token %gen.yield.2.save, i1 false)
  switch i8 %gen.yield.2.susp, label %coro.suspend [
    i8 0, label %gen.yield.2.resume
    i8 1, label %coro.cleanup
  ]

gen.yield.2.resume:                               ; preds = %gen.yield.1.resume
  br label %coro.final
}

define void @__sad_main() {
entry:
  %calltmp = call ptr @"\D8\A8\D8\B3\D9\8A\D8\B7"()
  %gen.sum = alloca i64, align 8
  store i64 0, ptr %gen.sum, align 8
  br label %gen.loop

gen.loop:                                         ; preds = %gen.yield, %entry
  call void @llvm.coro.resume(ptr %calltmp)
  %gen.isdone = call i1 @llvm.coro.done(ptr %calltmp)
  br i1 %gen.isdone, label %gen.end, label %gen.yield

gen.yield:                                        ; preds = %gen.loop
  %gen.promise = call ptr @llvm.coro.promise(ptr %calltmp, i32 8, i1 false)
  %gen.val = load i64, ptr %gen.promise, align 8
  %gen.cursum = load i64, ptr %gen.sum, align 8
  %gen.newsum = add i64 %gen.cursum, %gen.val
  store i64 %gen.newsum, ptr %gen.sum, align 8
  br label %gen.loop

gen.end:                                          ; preds = %gen.loop
  call void @llvm.coro.destroy(ptr %calltmp)
  %gen.result = load i64, ptr %gen.sum, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %gen.result)
  %calltmp1 = call ptr @"\D8\B6\D8\B9\D9\81"(i64 21)
  %gen.sum2 = alloca i64, align 8
  store i64 0, ptr %gen.sum2, align 8
  br label %gen.loop3

gen.loop3:                                        ; preds = %gen.yield4, %gen.end
  call void @llvm.coro.resume(ptr %calltmp1)
  %gen.isdone6 = call i1 @llvm.coro.done(ptr %calltmp1)
  br i1 %gen.isdone6, label %gen.end5, label %gen.yield4

gen.yield4:                                       ; preds = %gen.loop3
  %gen.promise7 = call ptr @llvm.coro.promise(ptr %calltmp1, i32 8, i1 false)
  %gen.val8 = load i64, ptr %gen.promise7, align 8
  %gen.cursum9 = load i64, ptr %gen.sum2, align 8
  %gen.newsum10 = add i64 %gen.cursum9, %gen.val8
  store i64 %gen.newsum10, ptr %gen.sum2, align 8
  br label %gen.loop3

gen.end5:                                         ; preds = %gen.loop3
  call void @llvm.coro.destroy(ptr %calltmp1)
  %gen.result11 = load i64, ptr %gen.sum2, align 8
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %gen.result11)
  %calltmp12 = call ptr @"\D9\85\D8\AC\D9\85\D9\88\D8\B9"()
  %gen.sum13 = alloca i64, align 8
  store i64 0, ptr %gen.sum13, align 8
  br label %gen.loop14

gen.loop14:                                       ; preds = %gen.yield15, %gen.end5
  call void @llvm.coro.resume(ptr %calltmp12)
  %gen.isdone17 = call i1 @llvm.coro.done(ptr %calltmp12)
  br i1 %gen.isdone17, label %gen.end16, label %gen.yield15

gen.yield15:                                      ; preds = %gen.loop14
  %gen.promise18 = call ptr @llvm.coro.promise(ptr %calltmp12, i32 8, i1 false)
  %gen.val19 = load i64, ptr %gen.promise18, align 8
  %gen.cursum20 = load i64, ptr %gen.sum13, align 8
  %gen.newsum21 = add i64 %gen.cursum20, %gen.val19
  store i64 %gen.newsum21, ptr %gen.sum13, align 8
  br label %gen.loop14

gen.end16:                                        ; preds = %gen.loop14
  call void @llvm.coro.destroy(ptr %calltmp12)
  %gen.result22 = load i64, ptr %gen.sum13, align 8
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %gen.result22)
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: read)
declare token @llvm.coro.id(i32, ptr readnone, ptr nocapture readonly, ptr) #1

; Function Attrs: nounwind memory(none)
declare i64 @llvm.coro.size.i64() #2

declare ptr @malloc(i64)

; Function Attrs: nounwind
declare ptr @llvm.coro.begin(token, ptr writeonly) #3

; Function Attrs: nomerge nounwind
declare token @llvm.coro.save(ptr) #4

; Function Attrs: nounwind
declare i8 @llvm.coro.suspend(token, i1) #3

; Function Attrs: nounwind memory(argmem: read)
declare ptr @llvm.coro.free(token, ptr nocapture readonly) #5

declare void @free(ptr)

; Function Attrs: nounwind
declare i1 @llvm.coro.end(ptr, i1, token) #3

declare void @llvm.coro.resume(ptr)

; Function Attrs: nounwind memory(argmem: readwrite)
declare i1 @llvm.coro.done(ptr nocapture readonly) #6

; Function Attrs: nounwind memory(none)
declare ptr @llvm.coro.promise(ptr nocapture, i32, i1) #2

declare void @llvm.coro.destroy(ptr)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}

attributes #0 = { presplitcoroutine }
attributes #1 = { nocallback nofree nosync nounwind willreturn memory(argmem: read) }
attributes #2 = { nounwind memory(none) }
attributes #3 = { nounwind }
attributes #4 = { nomerge nounwind }
attributes #5 = { nounwind memory(argmem: read) }
attributes #6 = { nounwind memory(argmem: readwrite) }
