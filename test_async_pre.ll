; ModuleID = 'test_async_sadc'
source_filename = "test_async_sadc"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@fmt.d = private unnamed_addr constant [5 x i8] c"%lld\00", align 1

define ptr @"\D8\AD\D8\B3\D8\A7\D8\A8"() #0 {
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

coro.final:                                       ; preds = %coro.init.resume
  %coro.final.save = call token @llvm.coro.save(ptr %coro.hdl)
  %coro.final.susp = call i8 @llvm.coro.suspend(token %coro.final.save, i1 true)
  switch i8 %coro.final.susp, label %coro.suspend [
    i8 0, label %coro.suspend
    i8 1, label %coro.cleanup
  ]

coro.cleanup:                                     ; preds = %coro.final, %entry
  %coro.mem = call ptr @llvm.coro.free(token %coro.id, ptr %coro.hdl)
  call void @free(ptr %coro.mem)
  br label %coro.suspend

coro.suspend:                                     ; preds = %coro.cleanup, %coro.final, %coro.final, %entry
  %0 = call i1 @llvm.coro.end(ptr %coro.hdl, i1 false, token none)
  ret ptr %coro.hdl

coro.init.resume:                                 ; preds = %entry
  store i64 42, ptr %coro.promise, align 8
  br label %coro.final
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %"%\D9\86" = alloca i64, align 8
  %calltmp = call ptr @"\D8\AD\D8\B3\D8\A7\D8\A8"()
  br label %await.0.loop

await.0.loop:                                     ; preds = %await.0.resume, %entry
  %await.0.isdone = call i1 @llvm.coro.done(ptr %calltmp)
  br i1 %await.0.isdone, label %await.0.done, label %await.0.resume

await.0.resume:                                   ; preds = %await.0.loop
  call void @llvm.coro.resume(ptr %calltmp)
  br label %await.0.loop

await.0.done:                                     ; preds = %await.0.loop
  %await.0.promise = call ptr @llvm.coro.promise(ptr %calltmp, i32 8, i1 false)
  %await.0.result = load i64, ptr %await.0.promise, align 8
  call void @llvm.coro.destroy(ptr %calltmp)
  br label %await.0.cont

await.0.cont:                                     ; preds = %await.0.done
  store i64 %await.0.result, ptr %"%\D9\86", align 8
  %"%\D9\86.load" = load i64, ptr %"%\D9\86", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D9\86.load")
  ret i64 0
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

; Function Attrs: nounwind memory(argmem: readwrite)
declare i1 @llvm.coro.done(ptr nocapture readonly) #6

declare void @llvm.coro.resume(ptr)

; Function Attrs: nounwind memory(none)
declare ptr @llvm.coro.promise(ptr nocapture, i32, i1) #2

declare void @llvm.coro.destroy(ptr)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}

attributes #0 = { "presplitcoroutine" }
attributes #1 = { nocallback nofree nosync nounwind willreturn memory(argmem: read) }
attributes #2 = { nounwind memory(none) }
attributes #3 = { nounwind }
attributes #4 = { nomerge nounwind }
attributes #5 = { nounwind memory(argmem: read) }
attributes #6 = { nounwind memory(argmem: readwrite) }
