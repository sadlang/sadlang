; ModuleID = 'test_async_sadc'
source_filename = "test_async_sadc"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%"\D8\AD\D8\B3\D8\A7\D8\A8_\D8\A8\D8\B3\D9\8A\D8\B7.Frame" = type { ptr, ptr, i64, i1 }
%"\D8\B6\D8\B9\D9\81.Frame" = type { ptr, ptr, i64, i64, i1 }
%"\D8\AC\D9\85\D8\B9.Frame" = type { ptr, ptr, i64, i64, i64, i1 }

@fmt.d = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@"\D8\AD\D8\B3\D8\A7\D8\A8_\D8\A8\D8\B3\D9\8A\D8\B7.resumers" = private constant [3 x ptr] [ptr @"\D8\AD\D8\B3\D8\A7\D8\A8_\D8\A8\D8\B3\D9\8A\D8\B7.resume", ptr @"\D8\AD\D8\B3\D8\A7\D8\A8_\D8\A8\D8\B3\D9\8A\D8\B7.destroy", ptr @"\D8\AD\D8\B3\D8\A7\D8\A8_\D8\A8\D8\B3\D9\8A\D8\B7.cleanup"]
@"\D8\B6\D8\B9\D9\81.resumers" = private constant [3 x ptr] [ptr @"\D8\B6\D8\B9\D9\81.resume", ptr @"\D8\B6\D8\B9\D9\81.destroy", ptr @"\D8\B6\D8\B9\D9\81.cleanup"]
@"\D8\AC\D9\85\D8\B9.resumers" = private constant [3 x ptr] [ptr @"\D8\AC\D9\85\D8\B9.resume", ptr @"\D8\AC\D9\85\D8\B9.destroy", ptr @"\D8\AC\D9\85\D8\B9.cleanup"]

define ptr @"\D8\AD\D8\B3\D8\A7\D8\A8_\D8\A8\D8\B3\D9\8A\D8\B7"() {
entry:
  %coro.promise = alloca i64, align 8
  store i64 0, ptr %coro.promise, align 8
  %coro.alloc = call ptr @malloc(i64 32)
  %resume.addr = getelementptr inbounds %"\D8\AD\D8\B3\D8\A7\D8\A8_\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.alloc, i32 0, i32 0
  store ptr @"\D8\AD\D8\B3\D8\A7\D8\A8_\D8\A8\D8\B3\D9\8A\D8\B7.resume", ptr %resume.addr, align 8
  %destroy.addr = getelementptr inbounds %"\D8\AD\D8\B3\D8\A7\D8\A8_\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.alloc, i32 0, i32 1
  store ptr @"\D8\AD\D8\B3\D8\A7\D8\A8_\D8\A8\D8\B3\D9\8A\D8\B7.destroy", ptr %destroy.addr, align 8
  %0 = getelementptr inbounds %"\D8\AD\D8\B3\D8\A7\D8\A8_\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.alloc, i32 0, i32 2
  %1 = load i64, ptr %coro.promise, align 8
  store i64 %1, ptr %0, align 8
  %coro.promise.reload.addr = getelementptr inbounds %"\D8\AD\D8\B3\D8\A7\D8\A8_\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.alloc, i32 0, i32 2
  %index.addr4 = getelementptr inbounds %"\D8\AD\D8\B3\D8\A7\D8\A8_\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.alloc, i32 0, i32 3
  store i1 false, ptr %index.addr4, align 1
  ret ptr %coro.alloc
}

define ptr @"\D8\B6\D8\B9\D9\81"(i64 %"\D9\86") {
entry:
  %"\D9\861" = alloca i64, align 8
  store i64 %"\D9\86", ptr %"\D9\861", align 8
  %coro.promise = alloca i64, align 8
  store i64 0, ptr %coro.promise, align 8
  %coro.alloc = call ptr @malloc(i64 40)
  %resume.addr = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.alloc, i32 0, i32 0
  store ptr @"\D8\B6\D8\B9\D9\81.resume", ptr %resume.addr, align 8
  %destroy.addr = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.alloc, i32 0, i32 1
  store ptr @"\D8\B6\D8\B9\D9\81.destroy", ptr %destroy.addr, align 8
  %0 = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.alloc, i32 0, i32 2
  %1 = load i64, ptr %coro.promise, align 8
  store i64 %1, ptr %0, align 8
  %2 = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.alloc, i32 0, i32 3
  %3 = load i64, ptr %"\D9\861", align 8
  store i64 %3, ptr %2, align 8
  %"\D9\861.reload.addr" = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.alloc, i32 0, i32 3
  %coro.promise.reload.addr = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.alloc, i32 0, i32 2
  %index.addr5 = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.alloc, i32 0, i32 4
  store i1 false, ptr %index.addr5, align 1
  ret ptr %coro.alloc
}

define ptr @"\D8\AC\D9\85\D8\B9"(i64 %"\D8\A3", i64 %"\D8\A8") {
entry:
  %"\D8\A82" = alloca i64, align 8
  store i64 %"\D8\A8", ptr %"\D8\A82", align 8
  %"\D8\A31" = alloca i64, align 8
  store i64 %"\D8\A3", ptr %"\D8\A31", align 8
  %coro.promise = alloca i64, align 8
  store i64 0, ptr %coro.promise, align 8
  %coro.alloc = call ptr @malloc(i64 48)
  %resume.addr = getelementptr inbounds %"\D8\AC\D9\85\D8\B9.Frame", ptr %coro.alloc, i32 0, i32 0
  store ptr @"\D8\AC\D9\85\D8\B9.resume", ptr %resume.addr, align 8
  %destroy.addr = getelementptr inbounds %"\D8\AC\D9\85\D8\B9.Frame", ptr %coro.alloc, i32 0, i32 1
  store ptr @"\D8\AC\D9\85\D8\B9.destroy", ptr %destroy.addr, align 8
  %0 = getelementptr inbounds %"\D8\AC\D9\85\D8\B9.Frame", ptr %coro.alloc, i32 0, i32 2
  %1 = load i64, ptr %coro.promise, align 8
  store i64 %1, ptr %0, align 8
  %2 = getelementptr inbounds %"\D8\AC\D9\85\D8\B9.Frame", ptr %coro.alloc, i32 0, i32 3
  %3 = load i64, ptr %"\D8\A82", align 8
  store i64 %3, ptr %2, align 8
  %4 = getelementptr inbounds %"\D8\AC\D9\85\D8\B9.Frame", ptr %coro.alloc, i32 0, i32 4
  %5 = load i64, ptr %"\D8\A31", align 8
  store i64 %5, ptr %4, align 8
  %"\D8\A82.reload.addr" = getelementptr inbounds %"\D8\AC\D9\85\D8\B9.Frame", ptr %coro.alloc, i32 0, i32 3
  %"\D8\A31.reload.addr" = getelementptr inbounds %"\D8\AC\D9\85\D8\B9.Frame", ptr %coro.alloc, i32 0, i32 4
  %coro.promise.reload.addr = getelementptr inbounds %"\D8\AC\D9\85\D8\B9.Frame", ptr %coro.alloc, i32 0, i32 2
  %index.addr6 = getelementptr inbounds %"\D8\AC\D9\85\D8\B9.Frame", ptr %coro.alloc, i32 0, i32 5
  store i1 false, ptr %index.addr6, align 1
  ret ptr %coro.alloc
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %"%\D9\863" = alloca i64, align 8
  %"%\D9\862" = alloca i64, align 8
  %"%\D9\861" = alloca i64, align 8
  %calltmp = call ptr @"\D8\AD\D8\B3\D8\A7\D8\A8_\D8\A8\D8\B3\D9\8A\D8\B7"()
  br label %await.0.loop

await.0.loop:                                     ; preds = %await.0.resume, %entry
  %0 = load ptr, ptr %calltmp, align 8
  %1 = icmp eq ptr %0, null
  br i1 %1, label %await.0.done, label %await.0.resume

await.0.resume:                                   ; preds = %await.0.loop
  %2 = getelementptr inbounds { ptr, ptr }, ptr %calltmp, i32 0, i32 0
  %3 = load ptr, ptr %2, align 8
  call fastcc void %3(ptr %calltmp)
  br label %await.0.loop

await.0.done:                                     ; preds = %await.0.loop
  %4 = getelementptr inbounds i8, ptr %calltmp, i32 16
  %await.0.result = load i64, ptr %4, align 8
  %5 = getelementptr inbounds { ptr, ptr }, ptr %calltmp, i32 0, i32 1
  %6 = load ptr, ptr %5, align 8
  call fastcc void %6(ptr %calltmp)
  store i64 %await.0.result, ptr %"%\D9\861", align 8
  %"%\D9\861.load" = load i64, ptr %"%\D9\861", align 8
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D9\861.load")
  %calltmp1 = call ptr @"\D8\B6\D8\B9\D9\81"(i64 21)
  br label %await.1.loop

await.1.loop:                                     ; preds = %await.1.resume, %await.0.done
  %8 = load ptr, ptr %calltmp1, align 8
  %9 = icmp eq ptr %8, null
  br i1 %9, label %await.1.done, label %await.1.resume

await.1.resume:                                   ; preds = %await.1.loop
  %10 = getelementptr inbounds { ptr, ptr }, ptr %calltmp1, i32 0, i32 0
  %11 = load ptr, ptr %10, align 8
  call fastcc void %11(ptr %calltmp1)
  br label %await.1.loop

await.1.done:                                     ; preds = %await.1.loop
  %12 = getelementptr inbounds i8, ptr %calltmp1, i32 16
  %await.1.result = load i64, ptr %12, align 8
  %13 = getelementptr inbounds { ptr, ptr }, ptr %calltmp1, i32 0, i32 1
  %14 = load ptr, ptr %13, align 8
  call fastcc void %14(ptr %calltmp1)
  store i64 %await.1.result, ptr %"%\D9\862", align 8
  %"%\D9\862.load" = load i64, ptr %"%\D9\862", align 8
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D9\862.load")
  %calltmp2 = call ptr @"\D8\AC\D9\85\D8\B9"(i64 30, i64 12)
  br label %await.2.loop

await.2.loop:                                     ; preds = %await.2.resume, %await.1.done
  %16 = load ptr, ptr %calltmp2, align 8
  %17 = icmp eq ptr %16, null
  br i1 %17, label %await.2.done, label %await.2.resume

await.2.resume:                                   ; preds = %await.2.loop
  %18 = getelementptr inbounds { ptr, ptr }, ptr %calltmp2, i32 0, i32 0
  %19 = load ptr, ptr %18, align 8
  call fastcc void %19(ptr %calltmp2)
  br label %await.2.loop

await.2.done:                                     ; preds = %await.2.loop
  %20 = getelementptr inbounds i8, ptr %calltmp2, i32 16
  %await.2.result = load i64, ptr %20, align 8
  %21 = getelementptr inbounds { ptr, ptr }, ptr %calltmp2, i32 0, i32 1
  %22 = load ptr, ptr %21, align 8
  call fastcc void %22(ptr %calltmp2)
  store i64 %await.2.result, ptr %"%\D9\863", align 8
  %"%\D9\863.load" = load i64, ptr %"%\D9\863", align 8
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D9\863.load")
  ret i64 0
}

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: read)
declare token @llvm.coro.id(i32, ptr readnone, ptr nocapture readonly, ptr) #0

; Function Attrs: nounwind memory(none)
declare i64 @llvm.coro.size.i64() #1

declare ptr @malloc(i64)

; Function Attrs: nounwind
declare ptr @llvm.coro.begin(token, ptr writeonly) #2

; Function Attrs: nomerge nounwind
declare token @llvm.coro.save(ptr) #3

; Function Attrs: nounwind
declare i8 @llvm.coro.suspend(token, i1) #2

; Function Attrs: nounwind memory(argmem: read)
declare ptr @llvm.coro.free(token, ptr nocapture readonly) #4

declare void @free(ptr)

; Function Attrs: nounwind
declare i1 @llvm.coro.end(ptr, i1, token) #2

; Function Attrs: nounwind memory(argmem: readwrite)
declare i1 @llvm.coro.done(ptr nocapture readonly) #5

declare void @llvm.coro.resume(ptr)

; Function Attrs: nounwind memory(none)
declare ptr @llvm.coro.promise(ptr nocapture, i32, i1) #1

declare void @llvm.coro.destroy(ptr)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: read)
declare ptr @llvm.coro.subfn.addr(ptr nocapture readonly, i8) #0

define internal fastcc void @"\D8\AD\D8\B3\D8\A7\D8\A8_\D8\A8\D8\B3\D9\8A\D8\B7.resume"(ptr noundef nonnull align 8 dereferenceable(32) %coro.hdl) {
entry.resume:
  %coro.promise = alloca i64, align 8
  %coro.promise.reload.addr = getelementptr inbounds %"\D8\AD\D8\B3\D8\A7\D8\A8_\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.hdl, i32 0, i32 2
  br label %resume.entry

resume.0:                                         ; preds = %resume.entry
  br label %resume.0.landing

resume.0.landing:                                 ; preds = %resume.0
  br label %AfterCoroSuspend

AfterCoroSuspend:                                 ; preds = %resume.0.landing
  switch i8 0, label %coro.suspend [
    i8 0, label %coro.init.resume
    i8 1, label %coro.cleanup
  ]

CoroSave1:                                        ; preds = %coro.init.resume
  %ResumeFn.addr = getelementptr inbounds %"\D8\AD\D8\B3\D8\A7\D8\A8_\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.hdl, i32 0, i32 0
  store ptr null, ptr %ResumeFn.addr, align 8
  br label %CoroSuspend2

CoroSuspend2:                                     ; preds = %CoroSave1
  br label %resume.1.landing

resume.1.landing:                                 ; preds = %CoroSuspend2
  br label %AfterCoroSuspend3

AfterCoroSuspend3:                                ; preds = %resume.1.landing
  %cond = icmp eq i8 -1, 1
  br i1 %cond, label %coro.cleanup, label %coro.suspend

coro.cleanup:                                     ; preds = %AfterCoroSuspend3, %AfterCoroSuspend
  call void @free(ptr %coro.hdl)
  br label %coro.suspend

coro.suspend:                                     ; preds = %coro.cleanup, %AfterCoroSuspend3, %AfterCoroSuspend
  br label %CoroEnd

CoroEnd:                                          ; preds = %coro.suspend
  ret void

coro.init.resume:                                 ; preds = %AfterCoroSuspend
  store i64 42, ptr %coro.promise.reload.addr, align 8
  br label %CoroSave1

resume.entry:                                     ; preds = %entry.resume
  br label %resume.0
}

define internal fastcc void @"\D8\AD\D8\B3\D8\A7\D8\A8_\D8\A8\D8\B3\D9\8A\D8\B7.destroy"(ptr noundef nonnull align 8 dereferenceable(32) %coro.hdl) {
entry.destroy:
  %coro.promise = alloca i64, align 8
  %coro.promise.reload.addr = getelementptr inbounds %"\D8\AD\D8\B3\D8\A7\D8\A8_\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.hdl, i32 0, i32 2
  br label %resume.entry

resume.0:                                         ; preds = %Switch
  br label %resume.0.landing

resume.0.landing:                                 ; preds = %resume.0
  br label %AfterCoroSuspend

AfterCoroSuspend:                                 ; preds = %resume.0.landing
  switch i8 1, label %coro.suspend [
    i8 0, label %coro.init.resume
    i8 1, label %coro.cleanup
  ]

CoroSave1:                                        ; preds = %coro.init.resume
  %ResumeFn.addr = getelementptr inbounds %"\D8\AD\D8\B3\D8\A7\D8\A8_\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.hdl, i32 0, i32 0
  store ptr null, ptr %ResumeFn.addr, align 8
  br label %CoroSuspend2

CoroSuspend2:                                     ; preds = %CoroSave1
  br label %resume.1.landing

resume.1:                                         ; preds = %resume.entry
  br label %resume.1.landing

resume.1.landing:                                 ; preds = %resume.1, %CoroSuspend2
  %0 = phi i8 [ -1, %CoroSuspend2 ], [ 1, %resume.1 ]
  br label %AfterCoroSuspend3

AfterCoroSuspend3:                                ; preds = %resume.1.landing
  %cond = icmp eq i8 %0, 1
  br i1 %cond, label %coro.cleanup, label %coro.suspend

coro.cleanup:                                     ; preds = %AfterCoroSuspend3, %AfterCoroSuspend
  call void @free(ptr %coro.hdl)
  br label %coro.suspend

coro.suspend:                                     ; preds = %coro.cleanup, %AfterCoroSuspend3, %AfterCoroSuspend
  br label %CoroEnd

CoroEnd:                                          ; preds = %coro.suspend
  ret void

coro.init.resume:                                 ; preds = %AfterCoroSuspend
  store i64 42, ptr %coro.promise.reload.addr, align 8
  br label %CoroSave1

resume.entry:                                     ; preds = %entry.destroy
  %ResumeFn.addr1 = getelementptr inbounds %"\D8\AD\D8\B3\D8\A7\D8\A8_\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.hdl, i32 0, i32 0
  %1 = load ptr, ptr %ResumeFn.addr1, align 8
  %2 = icmp eq ptr %1, null
  br i1 %2, label %resume.1, label %Switch

Switch:                                           ; preds = %resume.entry
  br label %resume.0
}

define internal fastcc void @"\D8\AD\D8\B3\D8\A7\D8\A8_\D8\A8\D8\B3\D9\8A\D8\B7.cleanup"(ptr noundef nonnull align 8 dereferenceable(32) %coro.hdl) {
entry.cleanup:
  %coro.promise = alloca i64, align 8
  %coro.promise.reload.addr = getelementptr inbounds %"\D8\AD\D8\B3\D8\A7\D8\A8_\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.hdl, i32 0, i32 2
  br label %resume.entry

resume.0:                                         ; preds = %Switch
  br label %resume.0.landing

resume.0.landing:                                 ; preds = %resume.0
  br label %AfterCoroSuspend

AfterCoroSuspend:                                 ; preds = %resume.0.landing
  switch i8 1, label %coro.suspend [
    i8 0, label %coro.init.resume
    i8 1, label %coro.cleanup
  ]

CoroSave1:                                        ; preds = %coro.init.resume
  %ResumeFn.addr = getelementptr inbounds %"\D8\AD\D8\B3\D8\A7\D8\A8_\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.hdl, i32 0, i32 0
  store ptr null, ptr %ResumeFn.addr, align 8
  br label %CoroSuspend2

CoroSuspend2:                                     ; preds = %CoroSave1
  br label %resume.1.landing

resume.1:                                         ; preds = %resume.entry
  br label %resume.1.landing

resume.1.landing:                                 ; preds = %resume.1, %CoroSuspend2
  %0 = phi i8 [ -1, %CoroSuspend2 ], [ 1, %resume.1 ]
  br label %AfterCoroSuspend3

AfterCoroSuspend3:                                ; preds = %resume.1.landing
  %cond = icmp eq i8 %0, 1
  br i1 %cond, label %coro.cleanup, label %coro.suspend

coro.cleanup:                                     ; preds = %AfterCoroSuspend3, %AfterCoroSuspend
  call void @free(ptr null)
  br label %coro.suspend

coro.suspend:                                     ; preds = %coro.cleanup, %AfterCoroSuspend3, %AfterCoroSuspend
  br label %CoroEnd

CoroEnd:                                          ; preds = %coro.suspend
  ret void

coro.init.resume:                                 ; preds = %AfterCoroSuspend
  store i64 42, ptr %coro.promise.reload.addr, align 8
  br label %CoroSave1

resume.entry:                                     ; preds = %entry.cleanup
  %ResumeFn.addr1 = getelementptr inbounds %"\D8\AD\D8\B3\D8\A7\D8\A8_\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.hdl, i32 0, i32 0
  %1 = load ptr, ptr %ResumeFn.addr1, align 8
  %2 = icmp eq ptr %1, null
  br i1 %2, label %resume.1, label %Switch

Switch:                                           ; preds = %resume.entry
  br label %resume.0
}

define internal fastcc void @"\D8\B6\D8\B9\D9\81.resume"(ptr noundef nonnull align 8 dereferenceable(40) %coro.hdl) {
entry.resume:
  %coro.promise = alloca i64, align 8
  %"\D9\861" = alloca i64, align 8
  %"\D9\861.reload.addr" = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.hdl, i32 0, i32 3
  %coro.promise.reload.addr = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.hdl, i32 0, i32 2
  br label %resume.entry

resume.0:                                         ; preds = %resume.entry
  br label %resume.0.landing

resume.0.landing:                                 ; preds = %resume.0
  br label %AfterCoroSuspend

AfterCoroSuspend:                                 ; preds = %resume.0.landing
  switch i8 0, label %coro.suspend [
    i8 0, label %coro.init.resume
    i8 1, label %coro.cleanup
  ]

CoroSave2:                                        ; preds = %coro.init.resume
  %ResumeFn.addr = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.hdl, i32 0, i32 0
  store ptr null, ptr %ResumeFn.addr, align 8
  br label %CoroSuspend3

CoroSuspend3:                                     ; preds = %CoroSave2
  br label %resume.1.landing

resume.1.landing:                                 ; preds = %CoroSuspend3
  br label %AfterCoroSuspend4

AfterCoroSuspend4:                                ; preds = %resume.1.landing
  %cond = icmp eq i8 -1, 1
  br i1 %cond, label %coro.cleanup, label %coro.suspend

coro.cleanup:                                     ; preds = %AfterCoroSuspend4, %AfterCoroSuspend
  call void @free(ptr %coro.hdl)
  br label %coro.suspend

coro.suspend:                                     ; preds = %coro.cleanup, %AfterCoroSuspend4, %AfterCoroSuspend
  br label %CoroEnd

CoroEnd:                                          ; preds = %coro.suspend
  ret void

coro.init.resume:                                 ; preds = %AfterCoroSuspend
  %"%\D9\86.load" = load i64, ptr %"\D9\861.reload.addr", align 8
  %shltmp = shl i64 %"%\D9\86.load", 1
  store i64 %shltmp, ptr %coro.promise.reload.addr, align 8
  br label %CoroSave2

resume.entry:                                     ; preds = %entry.resume
  br label %resume.0
}

define internal fastcc void @"\D8\B6\D8\B9\D9\81.destroy"(ptr noundef nonnull align 8 dereferenceable(40) %coro.hdl) {
entry.destroy:
  %coro.promise = alloca i64, align 8
  %"\D9\861" = alloca i64, align 8
  %"\D9\861.reload.addr" = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.hdl, i32 0, i32 3
  %coro.promise.reload.addr = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.hdl, i32 0, i32 2
  br label %resume.entry

resume.0:                                         ; preds = %Switch
  br label %resume.0.landing

resume.0.landing:                                 ; preds = %resume.0
  br label %AfterCoroSuspend

AfterCoroSuspend:                                 ; preds = %resume.0.landing
  switch i8 1, label %coro.suspend [
    i8 0, label %coro.init.resume
    i8 1, label %coro.cleanup
  ]

CoroSave2:                                        ; preds = %coro.init.resume
  %ResumeFn.addr = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.hdl, i32 0, i32 0
  store ptr null, ptr %ResumeFn.addr, align 8
  br label %CoroSuspend3

CoroSuspend3:                                     ; preds = %CoroSave2
  br label %resume.1.landing

resume.1:                                         ; preds = %resume.entry
  br label %resume.1.landing

resume.1.landing:                                 ; preds = %resume.1, %CoroSuspend3
  %0 = phi i8 [ -1, %CoroSuspend3 ], [ 1, %resume.1 ]
  br label %AfterCoroSuspend4

AfterCoroSuspend4:                                ; preds = %resume.1.landing
  %cond = icmp eq i8 %0, 1
  br i1 %cond, label %coro.cleanup, label %coro.suspend

coro.cleanup:                                     ; preds = %AfterCoroSuspend4, %AfterCoroSuspend
  call void @free(ptr %coro.hdl)
  br label %coro.suspend

coro.suspend:                                     ; preds = %coro.cleanup, %AfterCoroSuspend4, %AfterCoroSuspend
  br label %CoroEnd

CoroEnd:                                          ; preds = %coro.suspend
  ret void

coro.init.resume:                                 ; preds = %AfterCoroSuspend
  %"%\D9\86.load" = load i64, ptr %"\D9\861.reload.addr", align 8
  %shltmp = shl i64 %"%\D9\86.load", 1
  store i64 %shltmp, ptr %coro.promise.reload.addr, align 8
  br label %CoroSave2

resume.entry:                                     ; preds = %entry.destroy
  %ResumeFn.addr1 = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.hdl, i32 0, i32 0
  %1 = load ptr, ptr %ResumeFn.addr1, align 8
  %2 = icmp eq ptr %1, null
  br i1 %2, label %resume.1, label %Switch

Switch:                                           ; preds = %resume.entry
  br label %resume.0
}

define internal fastcc void @"\D8\B6\D8\B9\D9\81.cleanup"(ptr noundef nonnull align 8 dereferenceable(40) %coro.hdl) {
entry.cleanup:
  %coro.promise = alloca i64, align 8
  %"\D9\861" = alloca i64, align 8
  %"\D9\861.reload.addr" = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.hdl, i32 0, i32 3
  %coro.promise.reload.addr = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.hdl, i32 0, i32 2
  br label %resume.entry

resume.0:                                         ; preds = %Switch
  br label %resume.0.landing

resume.0.landing:                                 ; preds = %resume.0
  br label %AfterCoroSuspend

AfterCoroSuspend:                                 ; preds = %resume.0.landing
  switch i8 1, label %coro.suspend [
    i8 0, label %coro.init.resume
    i8 1, label %coro.cleanup
  ]

CoroSave2:                                        ; preds = %coro.init.resume
  %ResumeFn.addr = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.hdl, i32 0, i32 0
  store ptr null, ptr %ResumeFn.addr, align 8
  br label %CoroSuspend3

CoroSuspend3:                                     ; preds = %CoroSave2
  br label %resume.1.landing

resume.1:                                         ; preds = %resume.entry
  br label %resume.1.landing

resume.1.landing:                                 ; preds = %resume.1, %CoroSuspend3
  %0 = phi i8 [ -1, %CoroSuspend3 ], [ 1, %resume.1 ]
  br label %AfterCoroSuspend4

AfterCoroSuspend4:                                ; preds = %resume.1.landing
  %cond = icmp eq i8 %0, 1
  br i1 %cond, label %coro.cleanup, label %coro.suspend

coro.cleanup:                                     ; preds = %AfterCoroSuspend4, %AfterCoroSuspend
  call void @free(ptr null)
  br label %coro.suspend

coro.suspend:                                     ; preds = %coro.cleanup, %AfterCoroSuspend4, %AfterCoroSuspend
  br label %CoroEnd

CoroEnd:                                          ; preds = %coro.suspend
  ret void

coro.init.resume:                                 ; preds = %AfterCoroSuspend
  %"%\D9\86.load" = load i64, ptr %"\D9\861.reload.addr", align 8
  %shltmp = shl i64 %"%\D9\86.load", 1
  store i64 %shltmp, ptr %coro.promise.reload.addr, align 8
  br label %CoroSave2

resume.entry:                                     ; preds = %entry.cleanup
  %ResumeFn.addr1 = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.hdl, i32 0, i32 0
  %1 = load ptr, ptr %ResumeFn.addr1, align 8
  %2 = icmp eq ptr %1, null
  br i1 %2, label %resume.1, label %Switch

Switch:                                           ; preds = %resume.entry
  br label %resume.0
}

define internal fastcc void @"\D8\AC\D9\85\D8\B9.resume"(ptr noundef nonnull align 8 dereferenceable(48) %coro.hdl) {
entry.resume:
  %coro.promise = alloca i64, align 8
  %"\D8\A31" = alloca i64, align 8
  %"\D8\A82" = alloca i64, align 8
  %"\D8\A82.reload.addr" = getelementptr inbounds %"\D8\AC\D9\85\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 3
  %"\D8\A31.reload.addr" = getelementptr inbounds %"\D8\AC\D9\85\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 4
  %coro.promise.reload.addr = getelementptr inbounds %"\D8\AC\D9\85\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 2
  br label %resume.entry

resume.0:                                         ; preds = %resume.entry
  br label %resume.0.landing

resume.0.landing:                                 ; preds = %resume.0
  br label %AfterCoroSuspend

AfterCoroSuspend:                                 ; preds = %resume.0.landing
  switch i8 0, label %coro.suspend [
    i8 0, label %coro.init.resume
    i8 1, label %coro.cleanup
  ]

CoroSave3:                                        ; preds = %coro.init.resume
  %ResumeFn.addr = getelementptr inbounds %"\D8\AC\D9\85\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 0
  store ptr null, ptr %ResumeFn.addr, align 8
  br label %CoroSuspend4

CoroSuspend4:                                     ; preds = %CoroSave3
  br label %resume.1.landing

resume.1.landing:                                 ; preds = %CoroSuspend4
  br label %AfterCoroSuspend5

AfterCoroSuspend5:                                ; preds = %resume.1.landing
  %cond = icmp eq i8 -1, 1
  br i1 %cond, label %coro.cleanup, label %coro.suspend

coro.cleanup:                                     ; preds = %AfterCoroSuspend5, %AfterCoroSuspend
  call void @free(ptr %coro.hdl)
  br label %coro.suspend

coro.suspend:                                     ; preds = %coro.cleanup, %AfterCoroSuspend5, %AfterCoroSuspend
  br label %CoroEnd

CoroEnd:                                          ; preds = %coro.suspend
  ret void

coro.init.resume:                                 ; preds = %AfterCoroSuspend
  %"%\D8\A3.load" = load i64, ptr %"\D8\A31.reload.addr", align 8
  %"%\D8\A8.load" = load i64, ptr %"\D8\A82.reload.addr", align 8
  %addtmp = add i64 %"%\D8\A3.load", %"%\D8\A8.load"
  store i64 %addtmp, ptr %coro.promise.reload.addr, align 8
  br label %CoroSave3

resume.entry:                                     ; preds = %entry.resume
  br label %resume.0
}

define internal fastcc void @"\D8\AC\D9\85\D8\B9.destroy"(ptr noundef nonnull align 8 dereferenceable(48) %coro.hdl) {
entry.destroy:
  %coro.promise = alloca i64, align 8
  %"\D8\A31" = alloca i64, align 8
  %"\D8\A82" = alloca i64, align 8
  %"\D8\A82.reload.addr" = getelementptr inbounds %"\D8\AC\D9\85\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 3
  %"\D8\A31.reload.addr" = getelementptr inbounds %"\D8\AC\D9\85\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 4
  %coro.promise.reload.addr = getelementptr inbounds %"\D8\AC\D9\85\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 2
  br label %resume.entry

resume.0:                                         ; preds = %Switch
  br label %resume.0.landing

resume.0.landing:                                 ; preds = %resume.0
  br label %AfterCoroSuspend

AfterCoroSuspend:                                 ; preds = %resume.0.landing
  switch i8 1, label %coro.suspend [
    i8 0, label %coro.init.resume
    i8 1, label %coro.cleanup
  ]

CoroSave3:                                        ; preds = %coro.init.resume
  %ResumeFn.addr = getelementptr inbounds %"\D8\AC\D9\85\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 0
  store ptr null, ptr %ResumeFn.addr, align 8
  br label %CoroSuspend4

CoroSuspend4:                                     ; preds = %CoroSave3
  br label %resume.1.landing

resume.1:                                         ; preds = %resume.entry
  br label %resume.1.landing

resume.1.landing:                                 ; preds = %resume.1, %CoroSuspend4
  %0 = phi i8 [ -1, %CoroSuspend4 ], [ 1, %resume.1 ]
  br label %AfterCoroSuspend5

AfterCoroSuspend5:                                ; preds = %resume.1.landing
  %cond = icmp eq i8 %0, 1
  br i1 %cond, label %coro.cleanup, label %coro.suspend

coro.cleanup:                                     ; preds = %AfterCoroSuspend5, %AfterCoroSuspend
  call void @free(ptr %coro.hdl)
  br label %coro.suspend

coro.suspend:                                     ; preds = %coro.cleanup, %AfterCoroSuspend5, %AfterCoroSuspend
  br label %CoroEnd

CoroEnd:                                          ; preds = %coro.suspend
  ret void

coro.init.resume:                                 ; preds = %AfterCoroSuspend
  %"%\D8\A3.load" = load i64, ptr %"\D8\A31.reload.addr", align 8
  %"%\D8\A8.load" = load i64, ptr %"\D8\A82.reload.addr", align 8
  %addtmp = add i64 %"%\D8\A3.load", %"%\D8\A8.load"
  store i64 %addtmp, ptr %coro.promise.reload.addr, align 8
  br label %CoroSave3

resume.entry:                                     ; preds = %entry.destroy
  %ResumeFn.addr1 = getelementptr inbounds %"\D8\AC\D9\85\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 0
  %1 = load ptr, ptr %ResumeFn.addr1, align 8
  %2 = icmp eq ptr %1, null
  br i1 %2, label %resume.1, label %Switch

Switch:                                           ; preds = %resume.entry
  br label %resume.0
}

define internal fastcc void @"\D8\AC\D9\85\D8\B9.cleanup"(ptr noundef nonnull align 8 dereferenceable(48) %coro.hdl) {
entry.cleanup:
  %coro.promise = alloca i64, align 8
  %"\D8\A31" = alloca i64, align 8
  %"\D8\A82" = alloca i64, align 8
  %"\D8\A82.reload.addr" = getelementptr inbounds %"\D8\AC\D9\85\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 3
  %"\D8\A31.reload.addr" = getelementptr inbounds %"\D8\AC\D9\85\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 4
  %coro.promise.reload.addr = getelementptr inbounds %"\D8\AC\D9\85\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 2
  br label %resume.entry

resume.0:                                         ; preds = %Switch
  br label %resume.0.landing

resume.0.landing:                                 ; preds = %resume.0
  br label %AfterCoroSuspend

AfterCoroSuspend:                                 ; preds = %resume.0.landing
  switch i8 1, label %coro.suspend [
    i8 0, label %coro.init.resume
    i8 1, label %coro.cleanup
  ]

CoroSave3:                                        ; preds = %coro.init.resume
  %ResumeFn.addr = getelementptr inbounds %"\D8\AC\D9\85\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 0
  store ptr null, ptr %ResumeFn.addr, align 8
  br label %CoroSuspend4

CoroSuspend4:                                     ; preds = %CoroSave3
  br label %resume.1.landing

resume.1:                                         ; preds = %resume.entry
  br label %resume.1.landing

resume.1.landing:                                 ; preds = %resume.1, %CoroSuspend4
  %0 = phi i8 [ -1, %CoroSuspend4 ], [ 1, %resume.1 ]
  br label %AfterCoroSuspend5

AfterCoroSuspend5:                                ; preds = %resume.1.landing
  %cond = icmp eq i8 %0, 1
  br i1 %cond, label %coro.cleanup, label %coro.suspend

coro.cleanup:                                     ; preds = %AfterCoroSuspend5, %AfterCoroSuspend
  call void @free(ptr null)
  br label %coro.suspend

coro.suspend:                                     ; preds = %coro.cleanup, %AfterCoroSuspend5, %AfterCoroSuspend
  br label %CoroEnd

CoroEnd:                                          ; preds = %coro.suspend
  ret void

coro.init.resume:                                 ; preds = %AfterCoroSuspend
  %"%\D8\A3.load" = load i64, ptr %"\D8\A31.reload.addr", align 8
  %"%\D8\A8.load" = load i64, ptr %"\D8\A82.reload.addr", align 8
  %addtmp = add i64 %"%\D8\A3.load", %"%\D8\A8.load"
  store i64 %addtmp, ptr %coro.promise.reload.addr, align 8
  br label %CoroSave3

resume.entry:                                     ; preds = %entry.cleanup
  %ResumeFn.addr1 = getelementptr inbounds %"\D8\AC\D9\85\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 0
  %1 = load ptr, ptr %ResumeFn.addr1, align 8
  %2 = icmp eq ptr %1, null
  br i1 %2, label %resume.1, label %Switch

Switch:                                           ; preds = %resume.entry
  br label %resume.0
}

attributes #0 = { nocallback nofree nosync nounwind willreturn memory(argmem: read) }
attributes #1 = { nounwind memory(none) }
attributes #2 = { nounwind }
attributes #3 = { nomerge nounwind }
attributes #4 = { nounwind memory(argmem: read) }
attributes #5 = { nounwind memory(argmem: readwrite) }
