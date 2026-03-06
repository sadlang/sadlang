; ModuleID = 'test_coro_minimal.ll'
source_filename = "test_coro_minimal.ll"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc19.42.34436"

%my_coro.Frame = type { ptr, ptr, i64, i1 }

@fmt.d = private unnamed_addr constant [4 x i8] c"%d\0A\00"
@msg.start = private unnamed_addr constant [19 x i8] c"Coroutine started\0A\00"
@msg.resumed = private unnamed_addr constant [19 x i8] c"Coroutine resumed\0A\00"
@msg.done = private unnamed_addr constant [16 x i8] c"Coroutine done\0A\00"
@msg.result = private unnamed_addr constant [12 x i8] c"Result: %d\0A\00"
@my_coro.resumers = private constant [3 x ptr] [ptr @my_coro.resume, ptr @my_coro.destroy, ptr @my_coro.cleanup]

declare i32 @printf(ptr, ...)

declare ptr @malloc(i64)

declare void @free(ptr)

define ptr @my_coro(i64 %input) {
entry:
  %result.alloca = alloca i64, align 8
  store i64 0, ptr %result.alloca, align 8
  %alloc = call ptr @malloc(i64 32)
  %resume.addr = getelementptr inbounds %my_coro.Frame, ptr %alloc, i32 0, i32 0
  store ptr @my_coro.resume, ptr %resume.addr, align 8
  %destroy.addr = getelementptr inbounds %my_coro.Frame, ptr %alloc, i32 0, i32 1
  store ptr @my_coro.destroy, ptr %destroy.addr, align 8
  %0 = getelementptr inbounds %my_coro.Frame, ptr %alloc, i32 0, i32 2
  %1 = load i64, ptr %result.alloca, align 8
  store i64 %1, ptr %0, align 8
  %result.alloca.reload.addr = getelementptr inbounds %my_coro.Frame, ptr %alloc, i32 0, i32 2
  %2 = call i32 (ptr, ...) @printf(ptr @msg.start)
  %partial = mul i64 %input, 2
  store i64 %partial, ptr %result.alloca.reload.addr, align 8
  %index.addr4 = getelementptr inbounds %my_coro.Frame, ptr %alloc, i32 0, i32 3
  store i1 false, ptr %index.addr4, align 1
  ret ptr %alloc
}

define i32 @main() {
entry:
  %hdl = call ptr @my_coro(i64 21)
  %0 = getelementptr inbounds { ptr, ptr }, ptr %hdl, i32 0, i32 0
  %1 = load ptr, ptr %0, align 8
  call fastcc void %1(ptr %hdl)
  %2 = getelementptr inbounds i8, ptr %hdl, i32 16
  %result = load i64, ptr %2, align 8
  %3 = call i32 (ptr, ...) @printf(ptr @msg.result, i64 %result)
  %4 = getelementptr inbounds { ptr, ptr }, ptr %hdl, i32 0, i32 1
  %5 = load ptr, ptr %4, align 8
  call fastcc void %5(ptr %hdl)
  ret i32 0
}

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: read)
declare token @llvm.coro.id(i32, ptr readnone, ptr nocapture readonly, ptr) #0

; Function Attrs: nounwind memory(none)
declare i64 @llvm.coro.size.i64() #1

; Function Attrs: nounwind
declare ptr @llvm.coro.begin(token, ptr writeonly) #2

; Function Attrs: nomerge nounwind
declare token @llvm.coro.save(ptr) #3

; Function Attrs: nounwind
declare i8 @llvm.coro.suspend(token, i1) #2

; Function Attrs: nounwind
declare i1 @llvm.coro.end(ptr, i1, token) #2

; Function Attrs: nounwind memory(argmem: read)
declare ptr @llvm.coro.free(token, ptr nocapture readonly) #4

declare void @llvm.coro.resume(ptr)

declare void @llvm.coro.destroy(ptr)

; Function Attrs: nounwind memory(argmem: readwrite)
declare i1 @llvm.coro.done(ptr nocapture readonly) #5

; Function Attrs: nounwind memory(none)
declare ptr @llvm.coro.promise(ptr nocapture, i32, i1) #1

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: read)
declare ptr @llvm.coro.subfn.addr(ptr nocapture readonly, i8) #0

define internal fastcc void @my_coro.resume(ptr noundef nonnull align 8 dereferenceable(32) %hdl) {
entry.resume:
  %result.alloca = alloca i64, align 8
  %result.alloca.reload.addr = getelementptr inbounds %my_coro.Frame, ptr %hdl, i32 0, i32 2
  br label %resume.entry

resume.0:                                         ; preds = %resume.entry
  br label %resume.0.landing

resume.0.landing:                                 ; preds = %resume.0
  br label %AfterCoroSuspend

AfterCoroSuspend:                                 ; preds = %resume.0.landing
  switch i8 0, label %suspend [
    i8 0, label %resume1
    i8 1, label %cleanup
  ]

resume1:                                          ; preds = %AfterCoroSuspend
  %0 = call i32 (ptr, ...) @printf(ptr @msg.resumed)
  %prev = load i64, ptr %result.alloca.reload.addr, align 8
  %final = add i64 %prev, 100
  store i64 %final, ptr %result.alloca.reload.addr, align 8
  %1 = call i32 (ptr, ...) @printf(ptr @msg.done)
  br label %CoroSave1

CoroSave1:                                        ; preds = %resume1
  %ResumeFn.addr = getelementptr inbounds %my_coro.Frame, ptr %hdl, i32 0, i32 0
  store ptr null, ptr %ResumeFn.addr, align 8
  br label %CoroSuspend2

CoroSuspend2:                                     ; preds = %CoroSave1
  br label %resume.1.landing

resume.1.landing:                                 ; preds = %CoroSuspend2
  br label %AfterCoroSuspend3

AfterCoroSuspend3:                                ; preds = %resume.1.landing
  %cond = icmp eq i8 -1, 1
  br i1 %cond, label %cleanup, label %suspend

cleanup:                                          ; preds = %AfterCoroSuspend3, %AfterCoroSuspend
  call void @free(ptr %hdl)
  br label %suspend

suspend:                                          ; preds = %cleanup, %AfterCoroSuspend3, %AfterCoroSuspend
  br label %CoroEnd

CoroEnd:                                          ; preds = %suspend
  ret void

resume.entry:                                     ; preds = %entry.resume
  br label %resume.0
}

define internal fastcc void @my_coro.destroy(ptr noundef nonnull align 8 dereferenceable(32) %hdl) {
entry.destroy:
  %result.alloca = alloca i64, align 8
  %result.alloca.reload.addr = getelementptr inbounds %my_coro.Frame, ptr %hdl, i32 0, i32 2
  br label %resume.entry

resume.0:                                         ; preds = %Switch
  br label %resume.0.landing

resume.0.landing:                                 ; preds = %resume.0
  br label %AfterCoroSuspend

AfterCoroSuspend:                                 ; preds = %resume.0.landing
  switch i8 1, label %suspend [
    i8 0, label %resume1
    i8 1, label %cleanup
  ]

resume1:                                          ; preds = %AfterCoroSuspend
  %0 = call i32 (ptr, ...) @printf(ptr @msg.resumed)
  %prev = load i64, ptr %result.alloca.reload.addr, align 8
  %final = add i64 %prev, 100
  store i64 %final, ptr %result.alloca.reload.addr, align 8
  %1 = call i32 (ptr, ...) @printf(ptr @msg.done)
  br label %CoroSave1

CoroSave1:                                        ; preds = %resume1
  %ResumeFn.addr = getelementptr inbounds %my_coro.Frame, ptr %hdl, i32 0, i32 0
  store ptr null, ptr %ResumeFn.addr, align 8
  br label %CoroSuspend2

CoroSuspend2:                                     ; preds = %CoroSave1
  br label %resume.1.landing

resume.1:                                         ; preds = %resume.entry
  br label %resume.1.landing

resume.1.landing:                                 ; preds = %resume.1, %CoroSuspend2
  %2 = phi i8 [ -1, %CoroSuspend2 ], [ 1, %resume.1 ]
  br label %AfterCoroSuspend3

AfterCoroSuspend3:                                ; preds = %resume.1.landing
  %cond = icmp eq i8 %2, 1
  br i1 %cond, label %cleanup, label %suspend

cleanup:                                          ; preds = %AfterCoroSuspend3, %AfterCoroSuspend
  call void @free(ptr %hdl)
  br label %suspend

suspend:                                          ; preds = %cleanup, %AfterCoroSuspend3, %AfterCoroSuspend
  br label %CoroEnd

CoroEnd:                                          ; preds = %suspend
  ret void

resume.entry:                                     ; preds = %entry.destroy
  %ResumeFn.addr1 = getelementptr inbounds %my_coro.Frame, ptr %hdl, i32 0, i32 0
  %3 = load ptr, ptr %ResumeFn.addr1, align 8
  %4 = icmp eq ptr %3, null
  br i1 %4, label %resume.1, label %Switch

Switch:                                           ; preds = %resume.entry
  br label %resume.0
}

define internal fastcc void @my_coro.cleanup(ptr noundef nonnull align 8 dereferenceable(32) %hdl) {
entry.cleanup:
  %result.alloca = alloca i64, align 8
  %result.alloca.reload.addr = getelementptr inbounds %my_coro.Frame, ptr %hdl, i32 0, i32 2
  br label %resume.entry

resume.0:                                         ; preds = %Switch
  br label %resume.0.landing

resume.0.landing:                                 ; preds = %resume.0
  br label %AfterCoroSuspend

AfterCoroSuspend:                                 ; preds = %resume.0.landing
  switch i8 1, label %suspend [
    i8 0, label %resume1
    i8 1, label %cleanup
  ]

resume1:                                          ; preds = %AfterCoroSuspend
  %0 = call i32 (ptr, ...) @printf(ptr @msg.resumed)
  %prev = load i64, ptr %result.alloca.reload.addr, align 8
  %final = add i64 %prev, 100
  store i64 %final, ptr %result.alloca.reload.addr, align 8
  %1 = call i32 (ptr, ...) @printf(ptr @msg.done)
  br label %CoroSave1

CoroSave1:                                        ; preds = %resume1
  %ResumeFn.addr = getelementptr inbounds %my_coro.Frame, ptr %hdl, i32 0, i32 0
  store ptr null, ptr %ResumeFn.addr, align 8
  br label %CoroSuspend2

CoroSuspend2:                                     ; preds = %CoroSave1
  br label %resume.1.landing

resume.1:                                         ; preds = %resume.entry
  br label %resume.1.landing

resume.1.landing:                                 ; preds = %resume.1, %CoroSuspend2
  %2 = phi i8 [ -1, %CoroSuspend2 ], [ 1, %resume.1 ]
  br label %AfterCoroSuspend3

AfterCoroSuspend3:                                ; preds = %resume.1.landing
  %cond = icmp eq i8 %2, 1
  br i1 %cond, label %cleanup, label %suspend

cleanup:                                          ; preds = %AfterCoroSuspend3, %AfterCoroSuspend
  call void @free(ptr null)
  br label %suspend

suspend:                                          ; preds = %cleanup, %AfterCoroSuspend3, %AfterCoroSuspend
  br label %CoroEnd

CoroEnd:                                          ; preds = %suspend
  ret void

resume.entry:                                     ; preds = %entry.cleanup
  %ResumeFn.addr1 = getelementptr inbounds %my_coro.Frame, ptr %hdl, i32 0, i32 0
  %3 = load ptr, ptr %ResumeFn.addr1, align 8
  %4 = icmp eq ptr %3, null
  br i1 %4, label %resume.1, label %Switch

Switch:                                           ; preds = %resume.entry
  br label %resume.0
}

attributes #0 = { nocallback nofree nosync nounwind willreturn memory(argmem: read) }
attributes #1 = { nounwind memory(none) }
attributes #2 = { nounwind }
attributes #3 = { nomerge nounwind }
attributes #4 = { nounwind memory(argmem: read) }
attributes #5 = { nounwind memory(argmem: readwrite) }
