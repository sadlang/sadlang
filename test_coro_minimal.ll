; Minimal LLVM coroutine test
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc19.42.34436"

@fmt.d = private unnamed_addr constant [4 x i8] c"%d\0A\00"
@msg.start = private unnamed_addr constant [19 x i8] c"Coroutine started\0A\00"
@msg.resumed = private unnamed_addr constant [19 x i8] c"Coroutine resumed\0A\00"
@msg.done = private unnamed_addr constant [16 x i8] c"Coroutine done\0A\00"
@msg.result = private unnamed_addr constant [12 x i8] c"Result: %d\0A\00"

declare i32 @printf(ptr, ...)
declare ptr @malloc(i64)
declare void @free(ptr)

define ptr @my_coro(i64 %input) presplitcoroutine {
entry:
  %result.alloca = alloca i64, align 8
  store i64 0, ptr %result.alloca, align 8
  %id = call token @llvm.coro.id(i32 0, ptr %result.alloca, ptr null, ptr null)
  %size = call i64 @llvm.coro.size.i64()
  %alloc = call ptr @malloc(i64 %size)
  %hdl = call ptr @llvm.coro.begin(token %id, ptr %alloc)
  call i32 (ptr, ...) @printf(ptr @msg.start)
  %partial = mul i64 %input, 2
  store i64 %partial, ptr %result.alloca, align 8
  %save1 = call token @llvm.coro.save(ptr %hdl)
  %susp1 = call i8 @llvm.coro.suspend(token %save1, i1 false)
  switch i8 %susp1, label %suspend [
    i8 0, label %resume1
    i8 1, label %cleanup
  ]
resume1:
  call i32 (ptr, ...) @printf(ptr @msg.resumed)
  %prev = load i64, ptr %result.alloca, align 8
  %final = add i64 %prev, 100
  store i64 %final, ptr %result.alloca, align 8
  call i32 (ptr, ...) @printf(ptr @msg.done)
  %save.final = call token @llvm.coro.save(ptr %hdl)
  %susp.final = call i8 @llvm.coro.suspend(token %save.final, i1 true)
  switch i8 %susp.final, label %suspend [
    i8 0, label %suspend
    i8 1, label %cleanup
  ]
cleanup:
  %mem = call ptr @llvm.coro.free(token %id, ptr %hdl)
  call void @free(ptr %mem)
  br label %suspend
suspend:
  call i1 @llvm.coro.end(ptr %hdl, i1 false, token none)
  ret ptr %hdl
}

define i32 @main() {
entry:
  %hdl = call ptr @my_coro(i64 21)
  call void @llvm.coro.resume(ptr %hdl)
  %promise = call ptr @llvm.coro.promise(ptr %hdl, i32 8, i1 false)
  %result = load i64, ptr %promise, align 8
  call i32 (ptr, ...) @printf(ptr @msg.result, i64 %result)
  call void @llvm.coro.destroy(ptr %hdl)
  ret i32 0
}

declare token @llvm.coro.id(i32, ptr readnone, ptr nocapture readonly, ptr)
declare i64 @llvm.coro.size.i64()
declare ptr @llvm.coro.begin(token, ptr writeonly)
declare token @llvm.coro.save(ptr)
declare i8 @llvm.coro.suspend(token, i1)
declare i1 @llvm.coro.end(ptr, i1, token)
declare ptr @llvm.coro.free(token, ptr nocapture readonly)
declare void @llvm.coro.resume(ptr)
declare void @llvm.coro.destroy(ptr)
declare i1 @llvm.coro.done(ptr)
declare ptr @llvm.coro.promise(ptr nocapture, i32, i1)
